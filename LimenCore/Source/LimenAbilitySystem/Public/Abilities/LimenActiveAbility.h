// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenAbilityBase.h"
#include "Engine/TimerHandle.h"
#include "LimenActiveAbility.generated.h"

class AController;
/**
 * 
 */
UCLASS(Abstract)
class LIMENABILITYSYSTEM_API ULimenActiveAbility : public ULimenAbilityBase
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAbilityActivationDelegate, ULimenActiveAbility*, Ability);

public:
	ULimenActiveAbility();

	virtual void Initialize(AActor* InOwner) override;
	
	/**
	 * @brief Called to activate an ability (e.g. jumping).
	 * @param Controller The controller that is controlling the pawn who activated this ability
	 * @param Pawn The pawn who activate this ability.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	void ActivateAbility(AController* Controller, APawn* Pawn);
	/**
	 * @brief Called to cancel an ability (e.g. jumping).
	 * @param Controller The controller that is controlling the pawn who canceled this ability
	 * @param Pawn The pawn who canceled this ability.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	void CancelAbility(AController* Controller, APawn* Pawn);
	/**
	 * @brief Sets the rules whether the ability can be activated.
	 * @return True if it can be activated, false if not.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	virtual bool CanActivateAbility() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	float GetFullCooldownTime() const;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	float GetRemainingCooldownTime() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	bool IsActive() const;
	
protected:
	/**
	 * @brief The cooldown time that must pass before activating this ability again
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float AbilityCooldown;
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="0"))
	float AbilityActivationDelay;
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	bool bIsOneShot;

	void SetCooldownOver();

	virtual void AbilityActivated(AController* Controller, APawn* Pawn);
	virtual void AbilityCancelled(AController* Controller, APawn* Pawn);

private:	
	FTimerHandle AbilityCooldownTimer;
	bool bIsCooldownOver;
	bool bIsActive;
	
	UPROPERTY()
	TWeakObjectPtr<USkeletalMeshComponent> PawnMesh;
	FTimerHandle AnimationTimerHandle;
	
	FTimerHandle AbilityTimerHandle;

	void AbilityActivated_Wrapper(AController* Controller, APawn* Pawn);
};
