// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/LimenUpgradable.h"
#include "LimenUpgradeManager.generated.h"


class ULimenUpgradeDataAsset;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENITEMSMITH_API ULimenUpgradeManager : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FUpgradeDelegate, ULimenUpgradeManager*, UpgradeManager, const EUpgradeResponse, Response);

public:
	UPROPERTY(BlueprintAssignable)
	FUpgradeDelegate OnUpgradeProcessed;
	
	ULimenUpgradeManager();
	virtual void OnComponentCreated() override;
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Upgrades")
	const TArray<TSoftObjectPtr<ULimenUpgradeDataAsset>>& GetUpgradesData() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Upgrades")
	virtual void Upgrade(APlayerController* Controller, APawn* Pawn, ULimenUpgradeDataAsset* Upgrade);
	UFUNCTION(BlueprintCallable, Category="Limen|Upgrades")
	virtual EUpgradeResponse CanUpgrade(APlayerController* Controller, APawn* Pawn, const ULimenUpgradeDataAsset* Upgrade);

	UFUNCTION(BlueprintCallable, Category="Limen|Upgrades")
	int32 GetCurrentUpgradeLevel(const ULimenUpgradeDataAsset* Upgrade) const;
	int32 GetCurrentUpgradeLevel(const TSoftObjectPtr<const ULimenUpgradeDataAsset>& Upgrade) const;
	
	/**
	 * @brief Getter for the cost of the next upgrade
	 * @param Upgrade The upgrade to get the value from
	 * @return The cost or -1 if there is no next available upgrade
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Upgrades")
	float GetNextUpgradeCost(const ULimenUpgradeDataAsset* Upgrade) const;

	UFUNCTION(BlueprintCallable, Category="Limen|Upgrades")
	bool IsAtMaxLevel(const ULimenUpgradeDataAsset* Upgrade) const;
protected:

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSoftObjectPtr<ULimenUpgradeDataAsset>> UpgradesData;
};
