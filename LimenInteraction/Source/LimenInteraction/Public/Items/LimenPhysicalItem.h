// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenItemBase.h"
#include "Interfaces/LimenUpgradable.h"
#include "ItemActions/LimenItemAction.h"
#include "LimenPhysicalItem.generated.h"

class ULimenUpgradeManager;

/**
 * Base class for items that have a physical representation after being picked up, for example, a weapon.
 * Not suitable for items like health potions, notes, etc.
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENINTERACTION_API ALimenPhysicalItem : public ALimenItemBase, public ILimenUpgradable
{
	GENERATED_BODY()

public:
	ALimenPhysicalItem();

	/// ILimenUpgradable
	virtual ULimenUpgradeManager* GetUpgradeManager_Implementation() const override;
	virtual const FText& GetDisplayName_Implementation() const override;
	virtual const FText& GetDescription_Implementation() const override;
	virtual UTexture2D* GetImage_Implementation() const override;
	virtual void Upgrade_Implementation(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade) override {}
	///

	template<typename T = APawn>
	T* GetOwningPawn() const
	{
		return Cast<T>(OwningPawn);
	}
	
protected:
	UPROPERTY(EditAnywhere, Category="Limen|Upgrades")
	TObjectPtr<ULimenUpgradeManager> Upgrades;
	
	UPROPERTY()
	TObjectPtr<AController> OwningController;
	UPROPERTY()
	TObjectPtr<APawn> OwningPawn;
	
	uint32 CurrentUpgrade;

	bool bIsHolstered;

	virtual void Interact(AController* InController, APawn* InPawn) override;
};
