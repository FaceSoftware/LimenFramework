// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenUpgrade.generated.h"

class ULimenUpgradeDataAsset;
/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LIMENITEMSMITH_API ULimenUpgrade : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void Upgrade(int Level, const ULimenUpgradeDataAsset* InUpgradeDataAsset, const TScriptInterface<ILimenUpgradable>& Upgradable);
};
