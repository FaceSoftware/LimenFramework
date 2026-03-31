// Copyright Face Software. All Rights Reserved.


#include "Components/LimenItemHoldComponent.h"

#include "Utils/LimenReplicationUtils.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ULimenItemHoldComponent::ULimenItemHoldComponent()
{
	SetIsReplicatedByDefault(true);
}

void ULimenItemHoldComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, WeaponData, FDoRepLifetimeParams(COND_None,
		REPNOTIFY_OnChanged, true))
}

void ULimenItemHoldComponent::Hold(const TScriptInterface<ILimenHoldableItem>& InPhysicalItem)
{
	AActor* ActorPtr = Cast<AActor>(InPhysicalItem.GetObject());
	if (!ActorPtr) { return; }

	NETWORK_PREDICTION(Agnostic_Hold(ActorPtr), Server_Hold(ActorPtr));
}

void ULimenItemHoldComponent::StopHolding()
{
	NETWORK_PREDICTION(Agnostic_StopHolding(), Server_StopHolding());
}

bool ULimenItemHoldComponent::IsHoldingSomething() const
{
	return WeaponData.PhysicalItem.Get() != nullptr;
}

TScriptInterface<ILimenHoldableItem> ULimenItemHoldComponent::GetItem() const
{
	return TScriptInterface<ILimenHoldableItem>(WeaponData.PhysicalItem.Get());
}

void ULimenItemHoldComponent::OnRep_WeaponData(const FLimenItemHoldComponent_ReplicatedData& OldWeaponData)
{
	if (WeaponData.PhysicalItem != OldWeaponData.PhysicalItem)
	{
		OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
	}
}

void ULimenItemHoldComponent::Server_Hold_Implementation(AActor* InItem)
{
	Agnostic_Hold(InItem);
}

void ULimenItemHoldComponent::Server_StopHolding_Implementation()
{
	Agnostic_StopHolding();
}

void ULimenItemHoldComponent::Agnostic_Hold(AActor* InItem)
{
	if (!InItem || InItem == WeaponData.PhysicalItem.Get())
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
		}
		return;
	}
	
	WeaponData.PreviousPhysicalItem = WeaponData.PhysicalItem.Get();
	WeaponData.PhysicalItem = InItem;
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
	}

	OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
}

void ULimenItemHoldComponent::Agnostic_StopHolding()
{
	WeaponData.PreviousPhysicalItem = WeaponData.PhysicalItem.Get();
	WeaponData.PhysicalItem = nullptr;
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
	}

	OnItemChanged.Broadcast(WeaponData.PreviousPhysicalItem.Get(), WeaponData.PhysicalItem.Get());
}
