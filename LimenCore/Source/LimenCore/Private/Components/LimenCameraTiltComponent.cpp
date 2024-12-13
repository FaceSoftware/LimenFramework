// Copyright Face Software. All Rights Reserved.


#include "Components/LimenCameraTiltComponent.h"

ULimenCameraTiltComponent::ULimenCameraTiltComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1 / 16;
	bAutoActivate = true;
	bUsePawnControlRotation = false;
	
	MaxCameraTilt = 20;
	CameraTiltRecoverSpeed = 30;
	CurrentTilt = 0;

}

void ULimenCameraTiltComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	
	bUsePawnControlRotation = false;
}

void ULimenCameraTiltComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerPawn = Cast<APawn>(GetOwner());
}

void ULimenCameraTiltComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (!IsActive())
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
	if (OwnerPawn != nullptr && OwnerPawn->GetController() != nullptr)
	{
		SetWorldRotation(OwnerPawn->GetController()->GetControlRotation());
	}
	
	AddRelativeRotation(FRotator(0, 0, CurrentTilt));
	PreviousComponentRotation = GetComponentRotation();
}

void ULimenCameraTiltComponent::NotifyYawInput(const float InputValue)
{
	CurrentTilt += InputValue;
}

void ULimenCameraTiltComponent::CalculateCurrentTilt(const float DeltaTime)
{
	const float NewTilt = CurrentTilt - CameraTiltRecoverSpeed * CurrentTilt * DeltaTime;
	if (FMath::IsNegativeOrNegativeZero(NewTilt))
	{
		CurrentTilt = FMath::Clamp(NewTilt, -MaxCameraTilt, 0.f);
	}
	else
	{
		CurrentTilt = FMath::Clamp(NewTilt, 0.f, MaxCameraTilt);
	}
}
