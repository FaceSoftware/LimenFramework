// Copyright Face Software. All Rights Reserved.


#include "Components/LimenDynamicDepthOfFieldComponent.h"

#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include "Camera/CameraComponent.h"
#include "Engine/HitResult.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"


ULimenDynamicDepthOfFieldComponent::ULimenDynamicDepthOfFieldComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 32.f;
	bTraceIgnoresOwner = true;

	bAutoActivate = true;
	CurrentCameraPostProcessSettings = nullptr;
	LineTraceChannel = ECC_Camera;
}

void ULimenDynamicDepthOfFieldComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwner()->GetComponents<UCameraComponent>(OwnerCameras);
	if (OwnerCameras.IsEmpty())
	{
		Deactivate();
		return;
	}

	if (bTraceIgnoresOwner)
	{
		QueryParams.AddIgnoredActor(GetOwner());
	}
	QueryParams.bTraceComplex = true;
	
	for (UCameraComponent* Camera : OwnerCameras)
	{
		Camera->OnComponentActivated.AddUniqueDynamic(this, &ThisClass::ActiveCameraChanged);
		if (Camera->IsActive())
		{
			ActiveCamera = Camera;
			CurrentCameraPostProcessSettings = &ActiveCamera->PostProcessSettings;
		}
	}
}

void ULimenDynamicDepthOfFieldComponent::TickComponent(float DeltaTime, ELevelTick TickType,
													   FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsAnyCameraViewTarget())
	{
		return;
	}

	const float Distance = GetLookAtDistance();
	if (CurrentCameraPostProcessSettings != nullptr)
	{
		CurrentCameraPostProcessSettings->DepthOfFieldScale = 1.f;
		CurrentCameraPostProcessSettings->DepthOfFieldFocalDistance = Distance;
		ActiveCamera->MarkRenderStateDirty();		
	}
}

void ULimenDynamicDepthOfFieldComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (CurrentCameraPostProcessSettings != nullptr)
	{
		CurrentCameraPostProcessSettings->bOverride_DepthOfFieldScale = true;
		CurrentCameraPostProcessSettings->bOverride_DepthOfFieldFocalDistance = true;
		ActiveCamera->MarkRenderStateDirty();		
	}
}

void ULimenDynamicDepthOfFieldComponent::Deactivate()
{
	if (CurrentCameraPostProcessSettings != nullptr)
	{
		CurrentCameraPostProcessSettings->bOverride_DepthOfFieldScale = false;
		CurrentCameraPostProcessSettings->bOverride_DepthOfFieldFocalDistance = false;
		ActiveCamera->MarkRenderStateDirty();
	}

	Super::Deactivate();
}

float ULimenDynamicDepthOfFieldComponent::GetLookAtDistance() const
{
	const FVector Start = ActiveCamera->GetComponentLocation();
	const FVector End = Start + ActiveCamera->GetForwardVector() * MaxFocalDistance;

	float OutDistance;
	FHitResult Hit;
	if (!GetWorld()->LineTraceSingleByChannel(Hit, Start, End, LineTraceChannel, QueryParams))
	{
		OutDistance = MaxFocalDistance;
	}
	else
	{
		OutDistance = Hit.Distance;
	}

	return OutDistance;
}

void ULimenDynamicDepthOfFieldComponent::ActiveCameraChanged(UActorComponent* Component, bool bReset)
{
	ActiveCamera = CastChecked<UCameraComponent>(Component);
	check(OwnerCameras.Contains(ActiveCamera))
	CurrentCameraPostProcessSettings = &ActiveCamera->PostProcessSettings;
}

bool ULimenDynamicDepthOfFieldComponent::IsAnyCameraViewTarget() const
{
	return GetWorld()->GetFirstPlayerController()->GetViewTarget() == GetOwner();
}
