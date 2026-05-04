// Copyright Face Software. All Rights Reserved.


#include "Abilities/LimenActiveAbility.h"

#include "TimerManager.h"
#include "Engine/World.h"


ULimenActiveAbility::ULimenActiveAbility() : Super()
{
	AbilityCooldown = 0;
	bIsCooldownOver = false;
	bIsActive = false;
	AbilityActivationDelay = 0;
	bIsOneShot = false;
	bStartEnabled = true;
	bIsEnabled = true;
}

void ULimenActiveAbility::Initialize(AActor* InOwner)
{
	Super::Initialize(InOwner);
	
	bStartEnabled ? Enable() : Disable();
	bIsCooldownOver = true;
}

void ULimenActiveAbility::ForceDeactivateAbility()
{
	Super::ForceDeactivateAbility();

	DeactivateAbility(nullptr, nullptr);
}

void ULimenActiveAbility::Enable()
{
	bIsEnabled = true;
}

void ULimenActiveAbility::Disable()
{
	bIsEnabled = false;
	ForceDeactivateAbility();
}

void ULimenActiveAbility::ActivateAbility(AController* Controller, APawn* Pawn)
{
	check(GetOwner()->HasAuthority())
	if (!CanActivateAbility()) return;
	
	bIsActive = true;
	if (FMath::IsNearlyZero(AbilityActivationDelay) || FMath::IsNegativeOrNegativeZero(AbilityActivationDelay))
	{
		AbilityActivated_Wrapper(Controller, Pawn);
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(AbilityTimerHandle, [this, Controller, Pawn]
		{
			AbilityActivated_Wrapper(Controller, Pawn);
		}
		, AbilityActivationDelay, false);
	}
}

void ULimenActiveAbility::DeactivateAbility(AController* Controller, APawn* Pawn)
{
	check(GetOwner()->HasAuthority())
	if (!CanDeactivateAbility()) return;

	bIsActive = false;
	GetWorld()->GetTimerManager().ClearTimer(AbilityTimerHandle);
	AbilityCancelled(Controller, Pawn);
	StartCooldownTimer();

	Multicast_AbilityDeactivated(Controller, Pawn);
}

bool ULimenActiveAbility::CanActivateAbility() const
{
	return IsInitialized() && IsEnabled() && !IsOnCooldown() && !IsActive();
}

bool ULimenActiveAbility::CanDeactivateAbility() const
{
	if (ShouldForcefullyDeactivate()) { return true; }
	
	return IsActive() && IsInitialized();
}

float ULimenActiveAbility::GetFullCooldownTime() const
{
	return AbilityCooldown;
}

float ULimenActiveAbility::GetRemainingCooldownTime() const
{
	if (bIsCooldownOver) return 0.f;

	check(GetWorld()->GetTimerManager().IsTimerActive(AbilityCooldownTimer));
	return GetWorld()->GetTimerManager().GetTimerRemaining(AbilityCooldownTimer);
}

bool ULimenActiveAbility::IsActive() const
{
	return bIsActive;
}

bool ULimenActiveAbility::IsEnabled() const
{
	return bIsEnabled;
}

bool ULimenActiveAbility::IsOnCooldown() const
{
	return !bIsCooldownOver;
}

void ULimenActiveAbility::SetCooldownOver()
{
	if (!bIsCooldownOver) bIsCooldownOver = true;
}

void ULimenActiveAbility::AbilityActivated(AController* Controller, APawn* Pawn)
{
}

void ULimenActiveAbility::AbilityCancelled(AController* Controller, APawn* Pawn)
{
}

void ULimenActiveAbility::AbilityActivated_Wrapper(AController* Controller, APawn* Pawn)
{
	AbilityActivated(Controller, Pawn);
	BP_OnAbilityActivated(Controller, Pawn);
	bIsCooldownOver = false;

	bIsActive = !bIsOneShot;
	if (bIsOneShot)
	{
		StartCooldownTimer();
	}

	Multicast_AbilityActivated(Controller, Pawn);
}

void ULimenActiveAbility::StartCooldownTimer()
{
	if (FMath::IsNearlyZero(AbilityCooldown) || AbilityCooldown < 0.f)
	{
		SetCooldownOver();
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(AbilityCooldownTimer, this, &ThisClass::SetCooldownOver, AbilityCooldown, false);
}

void ULimenActiveAbility::Multicast_AbilityActivated_Implementation(AController* Controller, APawn* Pawn)
{
	OnAbilityActivated.Broadcast(this);
}

void ULimenActiveAbility::Multicast_AbilityDeactivated_Implementation(AController* Controller, APawn* Pawn)
{
	OnAbilityDeactivated.Broadcast(this);
}
