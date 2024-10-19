// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenHumanoidCharacterBase.h"

#include "EnhancedInputComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/LimenStepsSoundComponent.h"


const FName ALimenHumanoidCharacterBase::StepsComponentName = TEXT("StepsSoundComponent");

ALimenHumanoidCharacterBase::ALimenHumanoidCharacterBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	StepsSoundComponent = CreateDefaultSubobject<ULimenStepsSoundComponent>(StepsComponentName);
	StepsSoundComponent->SetupAttachment(GetCapsuleComponent());
}

void ALimenHumanoidCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInput)

	EnhancedInput->BindAction(JumpInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this,
						  &ThisClass::JumpInput);
	EnhancedInput->BindAction(CrouchInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this,
							  &ThisClass::CrouchInput);
}

const TSoftObjectPtr<UInputAction>& ALimenHumanoidCharacterBase::GetJumpInputAction() const
{
	return JumpInputAction;
}

const TSoftObjectPtr<UInputAction>& ALimenHumanoidCharacterBase::GetCrouchInputAction() const
{
	return CrouchInputAction;
}

ULimenStepsSoundComponent* ALimenHumanoidCharacterBase::GetStepsSoundComponent() const
{
	return StepsSoundComponent.Get();
}

void ALimenHumanoidCharacterBase::ToggleCrouch()
{
	if (bIsCrouched)
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}

ULimenInteractionComponent* ALimenHumanoidCharacterBase::GetInteractionComponent() const
{
	return nullptr;
}

float ALimenHumanoidCharacterBase::ApplyPointDamage(AController* DamageInstigator, AActor* DamageCauser,
												const float DamageTaken)
{
	return 0;
}

float ALimenHumanoidCharacterBase::ApplyMaxDamage(AController* DamageInstigator, AActor* DamageCauser)
{
	return 0;
}

void ALimenHumanoidCharacterBase::JumpInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		Jump();
	}
	else
	{
		StopJumping();
	}
}

void ALimenHumanoidCharacterBase::CrouchInput(const FInputActionInstance& Instance)
{
	if (Instance.GetValue().Get<bool>())
	{
		Crouch();
	}
	else
	{
		UnCrouch();
	}
}
