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
	explicit ALimenPhysicalItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/// ILimenUpgradable
	virtual ULimenUpgradeManager* GetUpgradeManager_Implementation() const override;
	virtual const FText& GetDisplayName_Implementation() const override;
	virtual const FText& GetDescription_Implementation() const override;
	virtual UTexture2D* GetImage_Implementation() const override;
	virtual void Upgrade_Implementation(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade) override {}
	///

	UFUNCTION(BlueprintNativeEvent)
	void SetDroppedState();
	virtual void SetDroppedState_Implementation();
	
protected:
	UPROPERTY(EditAnywhere, Category="Limen|Upgrades")
	TObjectPtr<ULimenUpgradeManager> Upgrades;
	
	uint32 CurrentUpgrade;

	bool bIsHolstered;
};
