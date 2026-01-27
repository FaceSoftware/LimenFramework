// Copyright Face Software. All Rights Reserved.


#include "Components/LimenPhysicalItemHoldComponent.h"

#include "Utils/LimenReplicationUtils.h"
#include "Items/LimenPhysicalItem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


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
	NETWORK_PREDICTION(Agnostic_Hold(InPhysicalItem), Server_Hold(InPhysicalItem));
}

void ULimenPhysicalItemHoldComponent::StopHolding()
{
	NETWORK_PREDICTION(Agnostic_StopHolding(), Server_StopHolding());
}

bool ULimenPhysicalItemHoldComponent::IsHoldingSomething() const
{
	return WeaponData.PhysicalItem.Get() != nullptr;
}

ALimenPhysicalItem* ULimenPhysicalItemHoldComponent::GetPhysicalItem() const
{
	return WeaponData.PhysicalItem.Get();
}

void ULimenPhysicalItemHoldComponent::OnRep_WeaponData(
	const FLimenPhysicalItemHoldComponent_ReplicatedWeaponData& OldWeaponData)
{
	if (WeaponData.PhysicalItem != OldWeaponData.PhysicalItem)
	{
		OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
	}
}

void ULimenPhysicalItemHoldComponent::Server_Hold_Implementation(ALimenPhysicalItem* InPhysicalItem)
{
	Agnostic_Hold(InPhysicalItem);
}

void ULimenPhysicalItemHoldComponent::Server_StopHolding_Implementation()
{
	Agnostic_StopHolding();
}

void ULimenPhysicalItemHoldComponent::Agnostic_Hold(ALimenPhysicalItem* InPhysicalItem)
{
	if (InPhysicalItem == nullptr || InPhysicalItem == WeaponData.PhysicalItem.Get())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
		return;
	}
	
	WeaponData.PreviousPhysicalItem = WeaponData.PhysicalItem.Get();
	WeaponData.PhysicalItem = InPhysicalItem;
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
	}

	OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
}

void ULimenPhysicalItemHoldComponent::Agnostic_StopHolding()
{
	WeaponData.PreviousPhysicalItem = WeaponData.PhysicalItem.Get();
	WeaponData.PhysicalItem = nullptr;
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
	}

	OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
}
