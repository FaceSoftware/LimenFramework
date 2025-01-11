// Copyright Face Software. All Rights Reserved.


#include "Characters/LimenGameplayCharacter.h"

#include "EnhancedInputComponent.h"
#include "Attributes/LimenHealthAttribute.h"
#include "Components/CapsuleComponent.h"
#include "Components/LimenAbilityComponent.h"
#include "Components/LimenStepsSoundComponent.h"


const FName ALimenGameplayCharacter::StepsComponentName = TEXT("StepsSoundComponent");

ALimenGameplayCharacter::ALimenGameplayCharacter(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	StepsSoundComponent = CreateDefaultSubobject<ULimenStepsSoundComponent>(StepsComponentName);
	StepsSoundComponent->SetupAttachment(GetCapsuleComponent());
}

void ALimenGameplayCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	check(EnhancedInput)


}

void ALimenGameplayCharacter::BeginPlay()
{
	Super::BeginPlay();

	HealthAttribute = GetAbilityComponent()->GetAttribute<ULimenHealthAttribute>();
	if (HealthAttribute != nullptr)
	{
		HealthAttribute->OnAttributeEmpty.AddUniqueDynamic(this, &ThisClass::OnHealthAttributeEmpty);
	}
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

void ALimenGameplayCharacter::Crouch(bool bClientSimulation)
{
	Super::Crouch(bClientSimulation);

	if (CanCrouch())
	{
		OnCharacterCrouched.Broadcast(true);
	}
}

float ALimenGameplayCharacter::ApplyPointDamage(AController* DamageInstigator, AActor* DamageCauser,
	const float DamageTaken, const FName& BoneName)
{
	float AbsoluteDamage = FMath::Abs(DamageTaken);

	if (BoneName != NAME_None || !BoneName.ToString().IsEmpty())
	{
		if (const float* Multiplier = DamageMultipliers.Find(BoneName); Multiplier != nullptr)
		{
			AbsoluteDamage *= *Multiplier;
		}
	}
	
	if (HealthAttribute != nullptr)
	{
		HealthAttribute->ModifyValueBy(-AbsoluteDamage);
	}
	
	DamageReceived(DamageInstigator, DamageCauser, AbsoluteDamage);
	return AbsoluteDamage;
}

float ALimenGameplayCharacter::ApplyMaxDamage(AController* DamageInstigator, AActor* DamageCauser)
{
	if (HealthAttribute != nullptr)
	{
		const float LeftOverValue = HealthAttribute->GetCurrentValue();
		HealthAttribute->SetCurrentValueAsMin();

		DamageReceived(DamageInstigator, DamageCauser, LeftOverValue);
		return LeftOverValue;
	}

	DamageReceived(DamageInstigator, DamageCauser, 0);
	return 0;
}

void ALimenGameplayCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	OnCharacterCrouched.Broadcast(false);
}

void ALimenGameplayCharacter::DamageReceived(AController* DamageInstigator, AActor* DamageCauser,
	const float DamageTaken)
{
}
