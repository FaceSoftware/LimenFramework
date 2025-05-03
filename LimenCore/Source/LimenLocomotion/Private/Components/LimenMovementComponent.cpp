// Copyright Face Software. All Rights Reserved.


#include "Components/LimenMovementComponent.h"


ULimenMovementComponent::ULimenMovementComponent()
{
	FastWalkSpeedMultiplier = 2.5f;
	CrouchFastWalkSpeedMultiplier = 2.5f;
	bFastMovementEnabledByDefault = false;
	bIsFastMovementEnabled = false;
}

ULimenMovementComponent::ULimenMovementComponent(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	FastWalkSpeedMultiplier = 2.5f;
	CrouchFastWalkSpeedMultiplier = 2.5f;
	bFastMovementEnabledByDefault = false;
	bIsFastMovementEnabled = false;
}

void ULimenMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	SetFastMovement(bFastMovementEnabledByDefault);
}

FNetworkPredictionData_Client* ULimenMovementComponent::GetPredictionData_Client() const
{
	// return Super::GetPredictionData_Client();

	if (!ClientPredictionData)
	{
		ULimenMovementComponent* MutableThis = const_cast<ULimenMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Limen(*this);
	}

	return ClientPredictionData;
}

void ULimenMovementComponent::SetFastMovement(const bool bEnabled)
{
	// check(GetOwner()->HasAuthority())
	
	if (bEnabled == bIsFastMovementEnabled)
	{
		return;
	}

	bIsFastMovementEnabled = bEnabled;
	OnFastMovementChanged();
}

bool ULimenMovementComponent::IsFastMovementEnabled() const
{
	return bIsFastMovementEnabled;
}

void ULimenMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);

	const bool bFastMovementEnabled = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
	SetFastMovement(bFastMovementEnabled); // Re-apply local state for prediction
}

void ULimenMovementComponent::OnFastMovementChanged()
{
	if (bIsFastMovementEnabled)
	{
		MaxWalkSpeedCrouched *= CrouchFastWalkSpeedMultiplier;
		MaxWalkSpeed *= FastWalkSpeedMultiplier;
	}
	else
	{
		MaxWalkSpeedCrouched /= CrouchFastWalkSpeedMultiplier;
		MaxWalkSpeed /= FastWalkSpeedMultiplier;
	}
}