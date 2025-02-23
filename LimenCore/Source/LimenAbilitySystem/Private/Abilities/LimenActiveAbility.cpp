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
}

void ULimenActiveAbility::Initialize(AActor* InOwner)
{
	Super::Initialize(InOwner);

	bIsCooldownOver = true;
	PawnMesh = InOwner->GetComponentByClass<USkeletalMeshComponent>();
}

void ULimenActiveAbility::ActivateAbility(AController* Controller, APawn* Pawn)
{
	check(CanActivateAbility());
	
	bIsActive = true;
	if (FMath::IsNearlyZero(AbilityActivationDelay))
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

void ULimenActiveAbility::CancelAbility(AController* Controller, APawn* Pawn)
{
	bIsActive = false;
	AbilityCancelled(Controller, Pawn);

	StartCooldownTimer();
}

bool ULimenActiveAbility::CanActivateAbility() const
{
	return bIsCooldownOver && !bIsActive;
}

float ULimenActiveAbility::GetFullCooldownTime() const
{
	return AbilityCooldown;
}

float ULimenActiveAbility::GetRemainingCooldownTime() const
{
	if (bIsCooldownOver)
	{
		return 0.f;
	}

	check(GetWorld()->GetTimerManager().IsTimerActive(AbilityCooldownTimer));
	return GetWorld()->GetTimerManager().GetTimerRemaining(AbilityCooldownTimer);
}

bool ULimenActiveAbility::IsActive() const
{
	return bIsActive;
}

void ULimenActiveAbility::SetCooldownOver()
{
	if (!bIsCooldownOver)
	{
		bIsCooldownOver = true;
	}
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
