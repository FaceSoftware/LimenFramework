// Copyright Face Software. All Rights Reserved.


#include "Components/LimenCameraComponent.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Engine/World.h"

ULimenCameraComponent::ULimenCameraComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 30.f;
	bAutoActivate = true;
	bUsePawnControlRotation = false;

	TiltFunction = ETiltFunction::EaseIn;
	MaxCameraTilt = 20;
	CameraTiltRecoverSpeed = 30;
	bInvertTilt = false;
	bEnableTilt = true;
	OriginalRelativeRotation = FRotator::ZeroRotator;
	CurrentTilt = 0;
	bInvertTilt = true;
	bIsTiltEnabled = false;

	bOriginalUsePawnControlRotation = false;
}

void ULimenCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	bOriginalUsePawnControlRotation = bUsePawnControlRotation;
	
	bIsTiltEnabled = bEnableTilt;
	SetTiltEnabled(bIsTiltEnabled);
	
	PlayerController = GetWorld()->GetFirstPlayerController();
	OriginalRelativeRotation = GetRelativeRotation();

	SetTiltFunctionPtr();
}

void ULimenCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType,
											  FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bIsTiltEnabled)
	{
		return;
	}

#if WITH_EDITOR
	
	if (!GetWorld()->IsGameWorld())
	{
		return;
	}
	
	TiltFunctionPtr.CheckCallable();
#endif

	TiltFunctionPtr(0.f);
	FRotator NewRotation = GetRelativeRotation();
	NewRotation.Roll = CurrentTilt;
	SetRelativeRotation(NewRotation);
}

void ULimenCameraComponent::SetTiltEnabled(const bool bEnabled)
{
	bIsTiltEnabled = bEnabled;
	if (bIsTiltEnabled)
	{
		bUsePawnControlRotation = false;
	}
	else
	{
		bUsePawnControlRotation = bOriginalUsePawnControlRotation;
	}
}

void ULimenCameraComponent::NotifyYawInput(const float InputValue)
{
	TiltFunctionPtr(bInvertTilt ? -InputValue : InputValue);
}

void ULimenCameraComponent::SetTiltFunctionPtr()
{
	switch (TiltFunction)
	{
	case ETiltFunction::Linear:
		TiltFunctionPtr = [this] (float Target) { CalculateLinearTilt(Target); };
		break;
	case ETiltFunction::EaseIn:
		TiltFunctionPtr = [this](float Target) { CalculateEaseInTilt(Target); };
		break;
	}
}

void ULimenCameraComponent::CalculateLinearTilt(const float Target)
{
	CurrentTilt = FMath::FInterpConstantTo(CurrentTilt, Target, GetWorld()->GetDeltaSeconds(), CameraTiltRecoverSpeed);
}

void ULimenCameraComponent::CalculateEaseInTilt(const float Target)
{
	CurrentTilt = FMath::FInterpTo(CurrentTilt, Target, GetWorld()->GetDeltaSeconds(), CameraTiltRecoverSpeed);
}
