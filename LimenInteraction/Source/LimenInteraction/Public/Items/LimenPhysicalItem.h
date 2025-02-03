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

#pragma region ILimenUpgradable
	
	virtual ULimenUpgradeManager* GetUpgradeManager_Implementation() const override;
	virtual const FText& GetDisplayName_Implementation() const override;
	virtual const FText& GetDescription_Implementation() const override;
	virtual UTexture2D* GetItemImage_Implementation() const override;
	virtual void Upgrade_Implementation(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade) override;

#pragma endregion 

	template<typename T = APawn>
	T* GetOwningPawn() const
	{
		static_assert(TIsDerivedFrom<T, APawn>::Value);
		return Cast<T>(OwningPawn.Get());
	}
	template<typename T = AController>
	T* GetOwningController() const
	{
		static_assert(TIsDerivedFrom<T, AController>::Value);
		return Cast<T>(OwningPawn.Get());
	}

	virtual void SetOwner(AActor* NewOwner) override;

protected:
	UPROPERTY(EditAnywhere, Category="Limen|Upgrades")
	TObjectPtr<ULimenUpgradeManager> Upgrades;

	virtual void Interact(AController* InController, APawn* InPawn) override;

private:
	UPROPERTY()
	TWeakObjectPtr<AController> OwningController;
	UPROPERTY()
	TWeakObjectPtr<APawn> OwningPawn;
	
	uint32 CurrentUpgrade;

	bool bIsHolstered;
};
