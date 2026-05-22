// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/DataTable.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Network/LimenNetworkUtils.h"
#include "LimenUpgradeShopComponent.generated.h"

struct FLimenUpgradeArray;
class AController;
class ULimenCreditsComponent;
class APlayerState;
class ULimenUpgrade;


USTRUCT()
struct FLimenUpgradeArrayItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	TSubclassOf<ULimenUpgrade> Upgrade;

	FLimenUpgradeArrayItem() = default;
};

USTRUCT()
struct FLimenUpgradeArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FLimenUpgradeArrayItem> Items;

	FLimenUpgradeArray() = default;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FastArrayDeltaSerialize<FLimenUpgradeArrayItem, FLimenUpgradeArray>(Items, DeltaParms,* this);
	}
};
DECLARE_STRUCT_OPS_TYPE_TRAITS(FLimenUpgradeArray)


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShopRerollDelegate, ULimenUpgradeShopComponent*, UpgradeShopComponent, const TArray<TSubclassOf<ULimenUpgrade>>&, UpgradesReroll);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENUPGRADES_API ULimenUpgradeShopComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FShopRerollDelegate OnShopUpdated;

	ULimenUpgradeShopComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	const TArray<TSubclassOf<ULimenUpgrade>>& GenerateAvailableUpgrades();
	const TArray<TSubclassOf<ULimenUpgrade>>& GetAllUpgrades() const;
	UFUNCTION(BlueprintCallable)
	bool CanRerollShop(const ULimenCreditsComponent* CreditsComponent) const;
	void RerollShop(ULimenCreditsComponent* CreditsComponent);
	UFUNCTION(BlueprintCallable, Category="Upgrades")
	int32 GetRerollCost() const;
	UFUNCTION(BlueprintCallable)
	void UpdateShop();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Upgrades", BlueprintReadOnly, meta=(InlineEditConditionToggle))
	bool bOnlyAllowSpecificQuantity;

	UPROPERTY(EditDefaultsOnly, Category="Upgrades", BlueprintReadOnly, meta=(ClampMin="1", EditCondition="bOnlyAllowSpecificQuantity"))
	int32 QuantityAvailable;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="Upgrades", BlueprintReadOnly, meta=(ClampMin="0", EditCondition="bOnlyAllowSpecificQuantity"))
	int32 RerollCost;

	UPROPERTY(EditDefaultsOnly, Category="Upgrades", BlueprintReadOnly)
	TArray<TSubclassOf<ULimenUpgrade>> AllUpgrades;

	virtual void ShopUpdated();

private:
	UPROPERTY(ReplicatedUsing="OnRep_LimitedAvailableUpgrades")
	mutable FLimenUpgradeArray ReplicatedLimitedAvailableUpgrades;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	mutable TArray<TSubclassOf<ULimenUpgrade>> LimitedAvailableUpgrades;

	UPROPERTY(Replicated)
	int32 Rerolls;
	
	void UpdateShopInternal() const;

	UFUNCTION()
	void OnRep_LimitedAvailableUpgrades();
};
