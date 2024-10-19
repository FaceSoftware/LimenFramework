// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenActiveAbility.h"
#include "LimenVariableMovementAbility.generated.h"


class ULimenStaminaAttribute;
class UCharacterMovementComponent;

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API ULimenVariableMovementAbility : public ULimenActiveAbility
{
	GENERATED_BODY()

public:	
	ULimenVariableMovementAbility();
	virtual void Initialize(AActor* InOwner) override;
	virtual void Tick(float DeltaTime) override;
	
	virtual void CancelAbility(AController* Controller, APawn* Pawn) override;
	virtual bool CanActivateAbility() const override;

protected:
	TObjectPtr<UCharacterMovementComponent> CharacterMovementComponent;
	bool bIsSprinting;

	virtual void AbilityActivated(AController* Controller, APawn* Pawn) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float SprintSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float CrouchSprintSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float WalkSpeed;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float CrouchWalkSpeed;
	/**
	 * @brief Amount of stamina that sprint drains each second
	 */
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	float SprintStaminaDrainRate;
	
	TObjectPtr<ULimenStaminaAttribute> Stamina;

	bool bWantsToSprint;

	void SetSprintMode();
	void SetWalkMode();
};
