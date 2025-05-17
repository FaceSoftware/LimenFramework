// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenUpgradeShopComponent.h"

#include "Components/LimenCreditsComponent.h"
#include "Net/UnrealNetwork.h"
#include "Subsystems/LimenGlobalRandomStreamSubsystem.h"
#include "Upgrades/LimenUpgrade.h"


ULimenUpgradeShopComponent::ULimenUpgradeShopComponent()
{
	bAutoActivate = true;
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);

	bOnlyAllowSpecificQuantity = true;
	QuantityAvailable = 4;
	RerollCost = 500;

	Rerolls = 0;
}

void ULimenUpgradeShopComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedLimitedAvailableUpgrades,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Rerolls,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

const TArray<TSubclassOf<ULimenUpgrade>>& ULimenUpgradeShopComponent::GenerateAvailableUpgrades()
{
	check(GetOwner()->HasAuthority())

	UpdateShopInternal();
	return LimitedAvailableUpgrades;
}

const TArray<TSubclassOf<ULimenUpgrade>>& ULimenUpgradeShopComponent::GetAllUpgrades() const
{
	return AllUpgrades;
}

bool ULimenUpgradeShopComponent::CanRerollShop(const ULimenCreditsComponent* CreditsComponent) const
{
	return CreditsComponent->GetCredits() >= RerollCost;
}

void ULimenUpgradeShopComponent::RerollShop(ULimenCreditsComponent* CreditsComponent)
{
	check(GetOwner()->HasAuthority())
	check(CanRerollShop(CreditsComponent))
	verify(CreditsComponent->WithdrawCredits(RerollCost))

	UpdateShopInternal();
	ShopUpdated();
}

int32 ULimenUpgradeShopComponent::GetRerollCost() const
{
	return RerollCost;
}

void ULimenUpgradeShopComponent::UpdateShop()
{
	check(GetOwner()->HasAuthority())
	UpdateShopInternal();
	ShopUpdated();
}

void ULimenUpgradeShopComponent::ShopUpdated()
{
	LimitedAvailableUpgrades.Empty(ReplicatedLimitedAvailableUpgrades.Items.Num());
	for (auto& Upgrade : ReplicatedLimitedAvailableUpgrades.Items)
	{
		LimitedAvailableUpgrades.Push(Upgrade.Upgrade);
	}
	
	OnShopUpdated.Broadcast(this, LimitedAvailableUpgrades);
}

void ULimenUpgradeShopComponent::UpdateShopInternal() const
{
	if (!bOnlyAllowSpecificQuantity || AllUpgrades.IsEmpty())
	{
		ReplicatedLimitedAvailableUpgrades.Items.Empty();
		ReplicatedLimitedAvailableUpgrades.MarkArrayDirty();
		return;
	}

	ReplicatedLimitedAvailableUpgrades.Items.Empty(QuantityAvailable);
	ReplicatedLimitedAvailableUpgrades.MarkArrayDirty();

	const int32 MaxIndex = AllUpgrades.Num() - 1;

	TArray<int32> RandomIndexes = ULimenGlobalRandomStreamSubsystem::Get()->GenerateRandomUniqueNumbers(
		MaxIndex, 0, QuantityAvailable);

	for (const auto RandomIndex : RandomIndexes)
	{
		FLimenUpgradeArrayItem Item;
		Item.Upgrade = AllUpgrades[RandomIndex];
		const int32 Index = ReplicatedLimitedAvailableUpgrades.Items.Add(Item);
		ReplicatedLimitedAvailableUpgrades.MarkItemDirty(ReplicatedLimitedAvailableUpgrades.Items[Index]);

		LimitedAvailableUpgrades.Push(AllUpgrades[RandomIndex]);
	}
}

void ULimenUpgradeShopComponent::OnRep_LimitedAvailableUpgrades()
{
	ShopUpdated();
}
