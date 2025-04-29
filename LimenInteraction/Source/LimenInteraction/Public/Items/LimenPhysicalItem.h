// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenItemBase.h"
#include "Interfaces/LimenUpgradable.h"
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
	explicit ALimenPhysicalItem(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

#pragma region ILimenUpgradable
	
	virtual ULimenUpgradeManager* GetUpgradeManager_Implementation() const override;
	virtual const FText& GetDisplayName_Implementation() const override;
	virtual const FText& GetDescription_Implementation() const override;
	virtual UTexture* GetItemImage_Implementation() const override;
	virtual void Upgrade_Implementation(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade) override;

#pragma endregion 
	
protected:
	UPROPERTY(EditAnywhere, Category="Limen|Upgrades")
	TObjectPtr<ULimenUpgradeManager> Upgrades;

private:	
	uint32 CurrentUpgrade;
	bool bIsHolstered;
};
