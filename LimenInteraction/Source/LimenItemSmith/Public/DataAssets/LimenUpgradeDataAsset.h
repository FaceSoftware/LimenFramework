// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenUpgradeDataAsset.generated.h"

class ULimenUpgrade;
/**
 * 
 */
UCLASS(BlueprintType)
class LIMENITEMSMITH_API ULimenUpgradeDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Unique name for this upgrade
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName UpgradeUniqueName;

	// The name of the upgrade displayed in the UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText UpgradeDisplayName;
	
	// A description of the upgrade to be displayed in UI
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText UpgradeDescription;
	
	// Index 0 corresponds to level 1, and so on...
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<float> LevelsCost;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ClampMin="0"))
	int StartLevel = 0;

	UPROPERTY(Transient)
	uint32 CurrentLevel = 0;
};
