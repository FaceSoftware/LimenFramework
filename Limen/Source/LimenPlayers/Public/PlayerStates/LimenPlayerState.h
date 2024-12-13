// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenBasePlayerState.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "Managers/LimenObjective.h"
#include "Items/LimenItemBase.h"
#include "LimenPlayerState.generated.h"

class ALimenPhysicalItem;
class ULimenInventoryComponent;

/**
 * 
 */
UCLASS()
class LIMENPLAYERS_API ALimenPlayerState : public ALimenBasePlayerState
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

	virtual void ObjectivesManagerInitialized(UObject* ObjectivesManagerObject);
	
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
