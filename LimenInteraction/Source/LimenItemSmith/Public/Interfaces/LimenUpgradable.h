// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/LimenUpgradeDataAsset.h"
#include "UObject/Interface.h"
#include "LimenUpgradable.generated.h"

class ULimenUpgradeManager;
// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class LIMENITEMSMITH_API ULimenUpgradable : public UInterface
{
	GENERATED_BODY()
};

class ULimenUpgradeDataAsset;

UENUM(BlueprintType)
enum class EUpgradeResponse : uint8
{
	Success,
	NotEnoughCredits,
	MaxLevelReached,
	UpgradeNotAvailable,
};

/**
 * 
 */
class LIMENITEMSMITH_API ILimenUpgradable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Limen|Upgradables")
	ULimenUpgradeManager* GetUpgradeManager() const;
	virtual ULimenUpgradeManager* GetUpgradeManager_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Limen|Upgradables")
	FText GetDisplayName() const;
	virtual const FText& GetDisplayName_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Limen|Upgradables")
	FText GetDescription() const;
	virtual const FText& GetDescription_Implementation() const = 0;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Limen|Upgradables")
	UTexture2D* GetItemImage() const;
	virtual UTexture2D* GetItemImage_Implementation() const = 0;

	// Implementation of upgrades. No validation should be done here, this should ignore any restrictions -> Use the upgrade manager for that.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Limen|Upgradables")
	void Upgrade(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade);
	virtual void Upgrade_Implementation(int32 NewLevel, ULimenUpgradeDataAsset* Upgrade) = 0;
};
