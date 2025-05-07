// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Pawn/LimenCharacterBase.h"
#include "LimenGameplayCharacter.generated.h"


class ULimenAttributeBase;
struct FDamageParameters;
class ULimenDamageType;
class ULimenDamageComponent;
class ULimenHealthAttribute;
class ULimenStepsSoundComponent;
class ULimenInteractionComponent;

UCLASS(Abstract)
class LIMENPLAYERS_API ALimenGameplayCharacter : public ALimenCharacterBase
{
	GENERATED_BODY()

public:
	static const FName StepsComponentName;
	
	UPROPERTY(BlueprintAssignable)
	FCharacterCrouchDelegate OnCharacterCrouched;
	
	explicit ALimenGameplayCharacter(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	
	ULimenStepsSoundComponent* GetStepsSoundComponent() const;

	/**
	 * @brief Toggles between Crouch and UnCrouch states
	 */
	void ToggleCrouch();
	virtual void Crouch(bool bClientSimulation = false) override;

	virtual void Interact() PURE_VIRTUAL(ALimenGameplayCharacter::Interact);
	virtual void StopInteraction() PURE_VIRTUAL(ALimenGameplayCharacter::Interact);

	UFUNCTION(BlueprintCallable, Category="Limen|Characters")
	virtual ULimenInteractionComponent* GetInteractionComponent() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<FName, float> DamageMultipliers;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULimenDamageComponent> DamageComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULimenStepsSoundComponent> StepsSoundComponent;
	
	virtual void OnEndCrouch(const float HalfHeightAdjust, const float ScaledHalfHeightAdjust) override;

	virtual float ProcessIncomingDamage(const FDamageParameters& InParams,
										const ULimenDamageType* InDamageType) const;
	
	UFUNCTION()
	virtual void DamageReceived(AController* InInstigator, AActor* InCauser,
								 TSubclassOf<ULimenDamageType> DamageType, const float Damage);

	UFUNCTION()
	virtual void HealthAttributeEmpty(ULimenAttributeBase* Attribute, const float NewValue);
	
private:
	TWeakObjectPtr<ULimenHealthAttribute> HealthAttribute;
};
