// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenCharacterBase.h"
#include "Interfaces/LimenDamageable.h"
#include "LimenHumanoidCharacterBase.generated.h"


class ULimenStepsSoundComponent;
class ULimenInteractionComponent;

UCLASS(Abstract)
class LIMENPLAYERS_API ALimenHumanoidCharacterBase : public ALimenCharacterBase, public ILimenDamageable
{
	GENERATED_BODY()

public:
	static const FName StepsComponentName;
	
	explicit ALimenHumanoidCharacterBase(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	const TSoftObjectPtr<UInputAction>& GetJumpInputAction() const;
	const TSoftObjectPtr<UInputAction>& GetCrouchInputAction() const;
	ULimenStepsSoundComponent* GetStepsSoundComponent() const;

	/**
	 * @brief Toggles between Crouch and UnCrouch
	 */
	void ToggleCrouch();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Characters")
	virtual ULimenInteractionComponent* GetInteractionComponent() const;

	virtual float ApplyPointDamage(AController* DamageInstigator, AActor* DamageCauser, const float DamageTaken) override;
	virtual float ApplyMaxDamage(AController* DamageInstigator, AActor* DamageCauser) override;
	
protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULimenStepsSoundComponent> StepsSoundComponent;
	/**
	 * @brief bool -> true = jump, false = Stop jumping
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> JumpInputAction;
	/**
	 * @brief bool -> true = crouch, false = Un-crouch
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Input Actions")
	TSoftObjectPtr<UInputAction> CrouchInputAction;
	
	virtual void JumpInput(const FInputActionInstance& Instance);
	virtual void CrouchInput(const FInputActionInstance& Instance);
	
private:
};
