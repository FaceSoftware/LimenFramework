// Copyright Face Software. All Rights Reserved.


#include "Components/LimenCameraTiltComponent.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Engine/World.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerController.h"

ULimenCameraTiltComponent::ULimenCameraTiltComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.f / 30.f;
	bAutoActivate = true;
	bUsePawnControlRotation = false;

	TargetFps = 60.f;
	TiltFunction = ETiltFunction::EaseIn;
	MaxCameraTilt = 20;
	CameraTiltRecoverSpeed = 30;
	bTiltTowardsMovement = false;
	bEnableTilt = true;
	CurrentTilt = 0;
	bIsTiltEnabled = true;

	bOriginalUsePawnControlRotation = false;
}

void ULimenCameraTiltComponent::BeginPlay()
{
	Super::BeginPlay();

	bOriginalUsePawnControlRotation = bUsePawnControlRotation;
	
	bIsTiltEnabled = bEnableTilt;
	SetTiltEnabled(bIsTiltEnabled);
	
	PlayerController = GetWorld()->GetFirstPlayerController();
	OriginalRelativeRotation = GetRelativeRotation();
}

void ULimenCameraTiltComponent::TickComponent(float DeltaTime, ELevelTick TickType,
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
	
#endif
	
	CalculateCurrentTilt(DeltaTime);
	FRotator NewRotation = GetRelativeRotation();
	NewRotation.Roll = CurrentTilt;
	SetRelativeRotation(NewRotation);
}

#if WITH_EDITORONLY_DATA
void ULimenCameraTiltComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.TickInterval = 1.f / TargetFps;
}
#endif

void ULimenCameraTiltComponent::SetTiltEnabled(const bool bEnabled)
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

void ULimenCameraTiltComponent::NotifyYawInput(const float InputValue)
{
	bTiltTowardsMovement ? CurrentTilt += InputValue : CurrentTilt -= InputValue;
}

void ULimenCameraTiltComponent::CalculateCurrentTilt(const float DeltaTime)
{
	switch (TiltFunction)
	{
	case ETiltFunction::Linear:
		CalculateLinearTilt(DeltaTime);
		break;
		
	case ETiltFunction::EaseIn:
		CalculateEaseInTilt(DeltaTime);
		break;
	}
}

void ULimenCameraTiltComponent::CalculateLinearTilt(const float DeltaTime)
{
	const float Decrement = CameraTiltRecoverSpeed * DeltaTime;
    
	if (CurrentTilt > 0.f)
	{
		CurrentTilt = FMath::Max(CurrentTilt - Decrement, 0.f);
	}
	else if (CurrentTilt < 0.f)
	{
		CurrentTilt = FMath::Min(CurrentTilt + Decrement, 0.f);
	}
}

void ULimenCameraTiltComponent::CalculateEaseInTilt(const float DeltaTime)
{
	CurrentTilt = FMath::FInterpTo(CurrentTilt, 0.f, DeltaTime, CameraTiltRecoverSpeed);
}
