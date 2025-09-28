// Copyright Face Software. All Rights Reserved.


#include "Components/LimenCameraComponent.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Rendering/MotionVectorSimulation.h"

ULimenCameraComponent::ULimenCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 30.f;
	bAutoActivate = true;
	bUsePawnControlRotation = false;

	TiltFunction = ETiltFunction::EaseIn;
	MaxCameraTilt = 20;
	TiltStrength = 1.f;
	CameraTiltRecoverSpeed = 30;
	bInvertTilt = false;
	bEnableTilt = true;
	OriginalRelativeRotation = FRotator::ZeroRotator;
	CurrentTilt = 0;
	bInvertTilt = true;
	bIsTiltEnabled = false;

	bOriginalUsePawnControlRotation = false;
	ZoomMultiplier = 1.f;
}

void ULimenCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	BaseCameraFOV = FieldOfView;
	BaseFirstPersonFOV = FirstPersonFieldOfView;

	bOriginalUsePawnControlRotation = bUsePawnControlRotation;
	
	bIsTiltEnabled = bEnableTilt;
	SetTiltEnabled(bIsTiltEnabled);
	
	PlayerController = GetWorld()->GetFirstPlayerController();
	OriginalRelativeRotation = GetRelativeRotation();

	SetTiltFunctionPtr();
}

void ULimenCameraComponent::GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView)
{
	if (IsXRHeadTrackedCamera())
	{
		HandleXRCamera(DeltaTime);
	}

	if (bUsePawnControlRotation)
	{
		const APawn* OwningPawn = Cast<APawn>(GetOwner());
		const AController* OwningController = OwningPawn ? OwningPawn->GetController() : nullptr;
		if (OwningController && OwningController->IsLocalPlayerController())
		{
			FRotator PawnViewRotation = OwningPawn->GetViewRotation();

			if (bIsTiltEnabled) PawnViewRotation.Roll = CurrentTilt = TiltFunctionPtr(0.f);

			if (!PawnViewRotation.Equals(GetComponentRotation()))
			{
				SetWorldRotation(PawnViewRotation);
			}
		}
	}
	else if (bIsTiltEnabled)
	{
		FRotator CurrentRelativeRotation = GetRelativeRotation();
		CurrentRelativeRotation.Roll = CurrentTilt = TiltFunctionPtr(0.f);
		SetRelativeRotation(CurrentRelativeRotation);
	}

	if (bUseAdditiveOffset)
	{
		FTransform OffsetCamToBaseCam = AdditiveOffset;
		FTransform BaseCamToWorld = GetComponentToWorld();
		FTransform OffsetCamToWorld = OffsetCamToBaseCam * BaseCamToWorld;

		DesiredView.Location = OffsetCamToWorld.GetLocation();
		DesiredView.Rotation = OffsetCamToWorld.Rotator();
	}
	else
	{
		DesiredView.Location = GetComponentLocation();
		DesiredView.Rotation = GetComponentRotation();
	}

	DesiredView.FOV = bUseAdditiveOffset ? (FieldOfView + AdditiveFOVOffset) : FieldOfView;
	DesiredView.AspectRatio = AspectRatio;
	DesiredView.bConstrainAspectRatio = bConstrainAspectRatio;
	DesiredView.bUseFieldOfViewForLOD = bUseFieldOfViewForLOD;
	DesiredView.ProjectionMode = ProjectionMode;
	DesiredView.OrthoWidth = OrthoWidth;
	DesiredView.OrthoNearClipPlane = OrthoNearClipPlane;
	DesiredView.OrthoFarClipPlane = OrthoFarClipPlane;
	DesiredView.bAutoCalculateOrthoPlanes = bAutoCalculateOrthoPlanes;
	DesiredView.AutoPlaneShift = AutoPlaneShift;
	DesiredView.bUpdateOrthoPlanes = bUpdateOrthoPlanes;
	DesiredView.bUseCameraHeightAsViewTarget = bUseCameraHeightAsViewTarget;
	DesiredView.FirstPersonFOV = bEnableFirstPersonFieldOfView ? FirstPersonFieldOfView : DesiredView.FOV;
	DesiredView.FirstPersonScale = bEnableFirstPersonScale ? FirstPersonScale : 1.0f;
	DesiredView.bUseFirstPersonParameters = bEnableFirstPersonFieldOfView || bEnableFirstPersonScale;
	
	DesiredView.ApplyOverscan(Overscan, bScaleResolutionWithOverscan, bCropOverscan);
	
	if (bAutoCalculateOrthoPlanes)
	{
		if (const AActor* ViewTarget = GetOwner())
		{
			DesiredView.SetCameraToViewTarget(ViewTarget->GetActorLocation());
		}
	}

	if (bOverrideAspectRatioAxisConstraint)
	{
		DesiredView.AspectRatioAxisConstraint = AspectRatioAxisConstraint;
	}

	// See if the CameraActor wants to override the PostProcess settings used.
	DesiredView.PostProcessBlendWeight = PostProcessBlendWeight;
	if (PostProcessBlendWeight > 0.0f)
	{
		DesiredView.PostProcessSettings = PostProcessSettings;
	}

	// If this camera component has a motion vector simulation transform, use that for the current view's previous transform
	DesiredView.PreviousViewTransform = FMotionVectorSimulation::Get().GetPreviousTransform(this);
}

void ULimenCameraComponent::SetTiltEnabled(const bool bEnabled)
{
	bIsTiltEnabled = bEnabled;
}

void ULimenCameraComponent::NotifyYawInput(const float InputValue)
{
	CurrentTilt = TiltFunctionPtr(TiltStrength * (bInvertTilt ? -InputValue : InputValue));
}

void ULimenCameraComponent::CameraZoom(const float Amount)
{
	const float PreviousZoom = FieldOfView;
	FieldOfView = FMath::Clamp(FieldOfView + Amount * ZoomMultiplier,
							   CameraZoomRange.GetLowerBoundValue(),
							   CameraZoomRange.GetUpperBoundValue());

	if (PreviousZoom != FieldOfView) OnCameraZoomValueChanged.Broadcast(this, FieldOfView);
}

void ULimenCameraComponent::SetCameraZoom(const float Amount)
{
	const float PreviousZoom = FieldOfView;
	FieldOfView = FMath::Clamp(Amount, CameraZoomRange.GetLowerBoundValue(), CameraZoomRange.GetUpperBoundValue());

	if (PreviousZoom != FieldOfView) OnCameraZoomValueChanged.Broadcast(this, FieldOfView);
}

void ULimenCameraComponent::FirstPersonZoom(const float Amount)
{
	const float PreviousZoom = FirstPersonFieldOfView;
	FirstPersonFieldOfView = FMath::Clamp(FirstPersonFieldOfView + Amount * ZoomMultiplier,
							   FirstPersonZoomRange.GetLowerBoundValue(),
							   FirstPersonZoomRange.GetUpperBoundValue());

	if (PreviousZoom != FirstPersonFieldOfView) OnFirstPersonZoomValueChanged.Broadcast(this, FirstPersonFieldOfView);
}

void ULimenCameraComponent::SetFirstPersonZoom(float Amount)
{
	const float PreviousZoom = FirstPersonFieldOfView;
	FirstPersonFieldOfView = FMath::Clamp(Amount, FirstPersonZoomRange.GetLowerBoundValue(), FirstPersonZoomRange.GetUpperBoundValue());

	if (PreviousZoom != FirstPersonFieldOfView) OnFirstPersonZoomValueChanged.Broadcast(this, FirstPersonFieldOfView);
}

FFloatRange ULimenCameraComponent::GetCameraZoomRange() const
{
	return CameraZoomRange;
}

float ULimenCameraComponent::GetCurrentCameraZoom() const
{
	return FieldOfView;
}

float ULimenCameraComponent::GetCurrentFirstPersonZoom() const
{
	return FirstPersonFieldOfView;
}

float ULimenCameraComponent::GetCurrentCameraZoomNormalized() const
{
	const float RangeAmount = CameraZoomRange.GetUpperBoundValue() - CameraZoomRange.GetLowerBoundValue();
	const float MinRangeAmount = CameraZoomRange.GetLowerBoundValue() / RangeAmount;
	return (FieldOfView / RangeAmount) - MinRangeAmount;
}

float ULimenCameraComponent::GetCurrentFirstPersonZoomNormalized() const
{
	const float RangeAmount = FirstPersonZoomRange.GetUpperBoundValue() - FirstPersonZoomRange.GetLowerBoundValue();
	const float MinRangeAmount = FirstPersonZoomRange.GetLowerBoundValue() / RangeAmount;
	return (FirstPersonFieldOfView / RangeAmount) - MinRangeAmount;
}

void ULimenCameraComponent::SetTiltFunctionPtr()
{
	switch (TiltFunction)
	{
	case ETiltFunction::Linear:
		TiltFunctionPtr = [this] (const float Target) { return CalculateLinearTilt(Target); };
		break;
	case ETiltFunction::EaseIn:
		TiltFunctionPtr = [this] (const float Target) { return CalculateEaseInTilt(Target); };
		break;
	}
}

float ULimenCameraComponent::CalculateLinearTilt(const float Target)
{
	return FMath::FInterpConstantTo(CurrentTilt, Target, GetWorld()->GetDeltaSeconds(), CameraTiltRecoverSpeed);
}

float ULimenCameraComponent::CalculateEaseInTilt(const float Target)
{
	return FMath::FInterpTo(CurrentTilt, Target, GetWorld()->GetDeltaSeconds(), CameraTiltRecoverSpeed);
}
