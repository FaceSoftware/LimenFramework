// Copyright Face Software. All Rights Reserved.


#include "Abilities/LimenHumanMovementAbility.h"

#include "Abilities/LimenVariableMovementAbility.h"
#include "Components/LimenCameraShakeComponent.h"
#include "Components/LimenStepsSoundComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LimenAbilityComponentTemplateDeclarations.h"


ULimenHumanMovementAbility::ULimenHumanMovementAbility()
{
	PreviousMovementMode = MOVE_None;
}

void ULimenHumanMovementAbility::Initialize(AActor* InOwner)
{
	Super::Initialize(InOwner);
	GetOwnerAbilityComponent()->InitializeAbilityDependency<ULimenVariableMovementAbility>();

	CameraShakeComponent = InOwner->GetComponentByClass<ULimenCameraShakeComponent>();
	CharacterMovementComponent = InOwner->GetComponentByClass<UCharacterMovementComponent>();
	StepsSoundComponent = InOwner->GetComponentByClass<ULimenStepsSoundComponent>();

	check(CharacterMovementComponent != nullptr);
}

void ULimenHumanMovementAbility::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	HandleCameraShake();

	PreviousMovementMode = CharacterMovementComponent->MovementMode;
}

bool ULimenHumanMovementAbility::IsStill() const
{
	return CharacterMovementComponent->Velocity.Length() <= UE_KINDA_SMALL_NUMBER;
}

void ULimenHumanMovementAbility::HandleCameraShake() const
{
	if (CharacterMovementComponent == nullptr || CameraShakeComponent == nullptr)
	{
		return;
	}
	
	if (CharacterMovementComponent->MovementMode == MOVE_Walking)
	{
		if (IsStill())
		{
			if (!CameraShakeComponent->IsPlayingStandCameraShake())
			{
				CameraShakeComponent->StopCameraShake();
				CameraShakeComponent->PlayStandCameraShake();
			}
		}
		else if (bIsSprinting)
		{
			if (!CameraShakeComponent->IsPlayingSprintCameraShake())
			{
				CameraShakeComponent->StopCameraShake();
				CameraShakeComponent->PlaySprintCameraShake();
			}
		}
		else
		{
			if (!CameraShakeComponent->IsPlayingWalkCameraShake())
			{
				CameraShakeComponent->StopCameraShake();
				CameraShakeComponent->PlayWalkCameraShake();
			}
		}
	}
}
