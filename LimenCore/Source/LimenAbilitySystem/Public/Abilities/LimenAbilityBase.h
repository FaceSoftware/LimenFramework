// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/LimenAbilityComponent.h"
#include "LimenStorage/Public/LimenStorageItem.h"
#include "LimenAbilityBase.generated.h"


class ULimenAbilityComponent;
class ULimenAttributeBase;
/**
 * 
 */
UCLASS(Abstract)
class LIMENABILITYSYSTEM_API ULimenAbilityBase : public ULimenStorageItem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	ULimenAbilityBase();
	
	/**
	 * @brief Called by the ability component after instancing this object
	 * @param InOwner The owner of this ability
	 */
	virtual void Initialize(AActor* InOwner);
	bool IsInitialized() const;
	/**
	 * @brief Forces the deactivation of the ability, useful if the player has died and the abilities should not persist.
	 * Should be overriden. Default implementation disables the Tick() function.
	 */
	virtual void ForceDeactivateAbility();

	/// FTickableGameObject Interface Start
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override final;
	virtual bool IsTickable() const override final;
	virtual bool IsAllowedToTick() const override final;
	virtual TStatId GetStatId() const override final;
	virtual bool IsTickableWhenPaused() const override final;
	virtual bool IsTickableInEditor() const override final;
	virtual UWorld* GetTickableGameObjectWorld() const override final;
	/// FTickableGameObject Interface End
	
protected:	
	ULimenAbilityComponent* GetOwnerAbilityComponent() const;	
	AActor* GetOwner() const;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void BP_OnAbilityActivated(AController* Controller, APawn* Pawn);

private:
	UPROPERTY()
	TObjectPtr<AActor> Owner;
	UPROPERTY()
	TObjectPtr<ULimenAbilityComponent> AbilityComponent;

	bool bIsInitialized;
	bool bShouldForcefullyDeactivate;
};
