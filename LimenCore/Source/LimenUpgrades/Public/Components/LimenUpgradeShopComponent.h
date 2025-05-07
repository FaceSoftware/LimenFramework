// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "LimenUpgradeShopComponent.generated.h"

class AController;
class ULimenCreditsComponent;
class APlayerState;
class ULimenUpgrade;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENUPGRADES_API ULimenUpgradeShopComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenUpgradeShopComponent();
	const TArray<TSubclassOf<ULimenUpgrade>>& GetAvailableUpgrades() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Upgrades", BlueprintReadOnly, meta=(InlineEditConditionToggle))
	bool bOnlyAllowSpecificQuantity;

	UPROPERTY(EditDefaultsOnly, Category="Upgrades", BlueprintReadOnly, meta=(ClampMin="1", EditCondition="bOnlyAllowSpecificQuantity"))
	int QuantityAvailable;

	UPROPERTY(EditDefaultsOnly, Category="Upgrades", BlueprintReadOnly)
	TArray<TSubclassOf<ULimenUpgrade>> AvailableUpgrades;

private:
	mutable TArray<TSubclassOf<ULimenUpgrade>> LimitedAvailableUpgrades;
};
