// Copyright Face Software. All Rights Reserved.


#include "Components/LimenPhysicalItemHoldComponent.h"

#include "Items/LimenPhysicalItem.h"
#include "Net/UnrealNetwork.h"


ULimenPhysicalItemHoldComponent::ULimenPhysicalItemHoldComponent()
{
	SetIsReplicatedByDefault(true);
}

void ULimenPhysicalItemHoldComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, WeaponData, FDoRepLifetimeParams(COND_None,
		REPNOTIFY_OnChanged, true))
}

void ULimenPhysicalItemHoldComponent::Hold(ALimenPhysicalItem* InPhysicalItem)
{
	check(GetOwner()->HasAuthority())

	if (InPhysicalItem == WeaponData.PhysicalItem.Get() || InPhysicalItem == nullptr)
	{
		return;
	}
	
	WeaponData.PreviousPhysicalItem = WeaponData.PhysicalItem.Get();
	WeaponData.PhysicalItem = InPhysicalItem;

	OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
}

void ULimenPhysicalItemHoldComponent::StopHolding()
{
	check(GetOwner()->HasAuthority())

	WeaponData.PreviousPhysicalItem = WeaponData.PhysicalItem.Get();
	WeaponData.PhysicalItem = nullptr;

	OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
}

bool ULimenPhysicalItemHoldComponent::IsHoldingSomething() const
{
	return IsValid(WeaponData.PhysicalItem.Get());
}

ALimenPhysicalItem* ULimenPhysicalItemHoldComponent::GetPhysicalItem() const
{
	return WeaponData.PhysicalItem.Get();
}

void ULimenPhysicalItemHoldComponent::OnRep_WeaponData()
{
	OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
}
