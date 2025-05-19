// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Managers/LimenObjective.h"
#include "Items/LimenItemBase.h"
#include "PlayerState/LimenPlayerStateBase.h"
#include "LimenPlayerState.generated.h"

class ALimenTool;
class ALimenWeapon;
class ALimenObjectivesManager;
class ALimenPhysicalItem;
class ULimenInventoryComponent;

/**
 * 
 */
UCLASS()
class LIMENPLAYERS_API ALimenPlayerState : public ALimenPlayerStateBase
{
	GENERATED_BODY()

public:	
	explicit ALimenPlayerState(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	
protected:
	UFUNCTION()
	virtual void CurrentHoldingToolChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New);
	UFUNCTION()
	virtual void CurrentHoldingWeaponChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New);
	UFUNCTION()
	virtual void InventoryUpdated(ULimenInventoryComponent* Inventory);
	UFUNCTION()
	virtual void ObjectiveAdded(ALimenObjective* Objective, const FObjectiveData& Data);
	UFUNCTION()
	virtual void ObjectiveUpdated(ALimenObjective* Objective, const FObjectiveData& Data);

	virtual void ObjectivesManagerInitialized(ALimenObjectivesManager* ObjectivesManager);
	
private:
	UPROPERTY()
	TObjectPtr<ALimenWeapon> CurrentHoldingWeapon;
	UPROPERTY()
	TObjectPtr<ALimenTool> CurrentHoldingTool;
	
	UPROPERTY()
	TArray<TSubclassOf<ALimenItemBase>> InventoryItems;
	UPROPERTY()
	TMap<TSubclassOf<ALimenObjective>, FObjectiveData> PlayerObjectives;

	UPROPERTY()
	TWeakObjectPtr<APlayerController> OwnerPlayerController;
};
