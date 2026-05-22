// Copyright Face Software. All Rights Reserved.


#include "Abilities/LimenVariableMovementAbility.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Attributes/LimenStaminaAttribute.h"
#include "Components/LimenAbilityComponent.h"


ULimenVariableMovementAbility::ULimenVariableMovementAbility()
{
	AbilityCooldown = 0.f;
	SprintSpeed = 800.f;
	CrouchSprintSpeed = 300.f;
	WalkSpeed = 400.f;
	CrouchWalkSpeed = 200.f;
	SprintStaminaDrainRate = 13.f;
	
	bWantsToSprint = false;
	bIsSprinting = false;
}

void ULimenVariableMovementAbility::Initialize(AActor* InOwner)
{
	Super::Initialize(InOwner);
	
	CharacterMovementComponent = InOwner->GetComponentByClass<UCharacterMovementComponent>();
	CharacterMovementComponent->SetMovementMode(MOVE_Walking);
	CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
	CharacterMovementComponent->MaxWalkSpeedCrouched = CrouchWalkSpeed;

	Stamina = GetOwnerAbilityComponent()->GetAttribute<ULimenStaminaAttribute>();
}

void ULimenVariableMovementAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsActive())
	{
		if (Stamina != nullptr)
		{
			Stamina->ModifyValueBy(-SprintStaminaDrainRate);
		}
	}

	if ((bIsSprinting && Stamina != nullptr && Stamina->IsEmpty()) || (!bWantsToSprint && bIsSprinting))
	{
		SetWalkMode();
	}
	else if (bWantsToSprint && !bIsSprinting)
	{
		SetSprintMode();
	}
}

bool ULimenVariableMovementAbility::CanActivateAbility() const
{
	if (!Super::CanActivateAbility())
	{
		return false;
	}

	if (CharacterMovementComponent == nullptr)
	{
		return false;
	}
	if (Stamina != nullptr && Stamina->IsEmpty())
	{
		return false;
	}
	
	return !FMath::IsNearlyZero(CharacterMovementComponent->GetLastUpdateVelocity().Length());
}

void ULimenVariableMovementAbility::AbilityActivated(AController* Controller, APawn* Pawn)
{
	Super::AbilityActivated(Controller, Pawn);

	bWantsToSprint = true;
}

void ULimenVariableMovementAbility::SetSprintMode()
{
	bIsSprinting = true;
	CharacterMovementComponent->MaxWalkSpeed = SprintSpeed;
	CharacterMovementComponent->MaxWalkSpeedCrouched = CrouchSprintSpeed;
}

void ULimenVariableMovementAbility::SetWalkMode()
{
	bIsSprinting = false;
	CharacterMovementComponent->MaxWalkSpeed = WalkSpeed;
	CharacterMovementComponent->MaxWalkSpeedCrouched = CrouchWalkSpeed;
}

void ULimenVariableMovementAbility::AbilityCancelled(AController* Controller, APawn* Pawn)
{
	Super::AbilityCancelled(Controller, Pawn);
	
	bWantsToSprint = false;
}
