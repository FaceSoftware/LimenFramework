// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "LimenStorageItem.h"
#include "LimenAbilityBase.generated.h"


class ULimenAbilityComponent;
class AController;
/**
 * 
 */
UCLASS(Abstract)
class LIMENABILITYSYSTEM_API ULimenAbilityBase : public ULimenStorageItem, public FTickableGameObject
{
	GENERATED_BODY()

public:
	ULimenAbilityBase();
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;

	
	/**
	 * @brief Called by the ability component after instancing this object
	 * @param InOwner The owner of this ability
	 */
	virtual void Initialize(AActor* InOwner);
	bool IsInitialized() const;
	virtual void Deinitialize(AActor* InOwner);
	/**
	 * @brief Forces the deactivation of the ability, useful if the player has died and the abilities should not persist.
	 * Should be overriden. Default implementation disables the Tick() function.
	 */
	virtual void ForceDeactivateAbility();
	
	bool ShouldForcefullyDeactivate() const;

#pragma region FTickableGameObject Interface Start

	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override final;
	virtual bool IsTickable() const override final;
	virtual TStatId GetStatId() const override final;
	virtual bool IsTickableWhenPaused() const override final;
	virtual bool IsTickableInEditor() const override final;
	virtual UWorld* GetTickableGameObjectWorld() const override final;

#pragma endregion
	
protected:	
	ULimenAbilityComponent* GetOwnerAbilityComponent() const;	
	AActor* GetOwner() const;
	template<typename T>
	T* GetOwner() const
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value);
		return Cast<T>(GetOwner());
	}
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void BP_OnAbilityActivated(AController* Controller, APawn* Pawn);

private:
	UPROPERTY()
	TWeakObjectPtr<AActor> Owner;
	UPROPERTY()
	TObjectPtr<ULimenAbilityComponent> AbilityComponent;

	bool bIsInitialized;
	bool bShouldForcefullyDeactivate;
};
