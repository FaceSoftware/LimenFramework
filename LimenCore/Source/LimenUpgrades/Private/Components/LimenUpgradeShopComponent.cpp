// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenUpgradeShopComponent.h"

#include "Components/LimenCreditsComponent.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "Upgrades/LimenUpgrade.h"


ULimenUpgradeShopComponent::ULimenUpgradeShopComponent()
{
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);

	bOnlyAllowSpecificQuantity = true;
	QuantityAvailable = 4;
}

const TArray<TSubclassOf<ULimenUpgrade>>& ULimenUpgradeShopComponent::GetAvailableUpgrades() const
{
	if (!bOnlyAllowSpecificQuantity)
	{
		return AvailableUpgrades;
	}
	
	LimitedAvailableUpgrades.Empty(QuantityAvailable);
	const int32 MaxIndex = AvailableUpgrades.Num() - 1;
	
	TArray<int32> RandomIndexes = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(
		MaxIndex, 0, QuantityAvailable);

	for (const auto RandomIndex : RandomIndexes)
	{
		LimitedAvailableUpgrades.Push(AvailableUpgrades[RandomIndex]);
	}

	return LimitedAvailableUpgrades;
}
