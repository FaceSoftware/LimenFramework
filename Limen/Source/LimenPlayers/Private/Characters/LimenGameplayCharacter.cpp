// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenGameplayCharacter.h"

#include "Attributes/LimenHealthAttribute.h"
#include "Components/CapsuleComponent.h"
#include "Components/LimenAbilityComponent.h"
#include "Components/LimenDamageComponent.h"
#include "Components/LimenStepsSoundComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


const FName ALimenGameplayCharacter::StepsComponentName = TEXT("StepsSoundComponent");

ALimenGameplayCharacter::ALimenGameplayCharacter(const FObjectInitializer& InObjectInitializer)
	: Super(InObjectInitializer)
{
	StepsSoundComponent = CreateDefaultSubobject<ULimenStepsSoundComponent>(StepsComponentName);
	StepsSoundComponent->SetupAttachment(GetCapsuleComponent());

	DamageComponent = CreateDefaultSubobject<ULimenDamageComponent>(TEXT("DamageComponent"));
}

void ALimenGameplayCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthAttribute = GetAbilityComponent()->GetAttribute<ULimenHealthAttribute>();
	if (HealthAttribute != nullptr)
	{
		HealthAttribute->OnAttributeEmpty.AddUniqueDynamic(this, &ThisClass::HealthAttributeEmpty);
	}

	DamageComponent->SetDamageCalculationFunction(this, &ThisClass::ProcessIncomingDamage);
	DamageComponent->OnDamageReceived.AddUniqueDynamic(this, &ALimenGameplayCharacter::DamageReceived);
}

ULimenStepsSoundComponent* ALimenGameplayCharacter::GetStepsSoundComponent() const
{
	return StepsSoundComponent.Get();
}

void ALimenGameplayCharacter::ToggleCrouch()
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

void ALimenGameplayCharacter::Crouch(const bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);

	if (CanCrouch())
	{
		OnCharacterCrouched.Broadcast(true);
	}
}

ULimenInteractionComponent* ALimenGameplayCharacter::GetInteractionComponent() const
{
	return nullptr;
}

void ALimenGameplayCharacter::OnEndCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	OnCharacterCrouched.Broadcast(false);
}

float ALimenGameplayCharacter::ProcessIncomingDamage(const FDamageParameters& InParams,
													 const ULimenDamageType* InDamageType) const
{
	const float* DamageMult = DamageMultipliers.Find(InParams.HitBoneName);
	if (!DamageMult) return InParams.DamageValue;

	return *DamageMult * InParams.DamageValue;
}

void ALimenGameplayCharacter::DamageReceived(AController* InInstigator, AActor* InCauser,
											 TSubclassOf<ULimenDamageType> DamageType, const float Damage)
{
	if (HealthAttribute.IsValid()) HealthAttribute->ModifyValueBy(-Damage);
}

void ALimenGameplayCharacter::HealthAttributeEmpty(ULimenAttributeBase* Attribute, const float NewValue)
{
	GetCharacterMovement()->DisableMovement();
	AbilityComponent->DeactivateAllAbilities();
}
