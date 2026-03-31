// Copyright Face Software. All Rights Reserved.


#include "Components/LimenHoldComponent.h"

#include "Utils/LimenReplicationUtils.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ULimenHoldComponent::ULimenHoldComponent()
{
	SetIsReplicatedByDefault(true);
}

void ULimenHoldComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, WeaponData, FDoRepLifetimeParams(COND_None,
		REPNOTIFY_OnChanged, true))
}

void ULimenHoldComponent::Hold(AActor* InActor)
{
	if (!InActor) { return; }

	NETWORK_PREDICTION(Agnostic_Hold(InActor), Server_Hold(InActor));
}

void ULimenHoldComponent::StopHolding()
{
	NETWORK_PREDICTION(Agnostic_StopHolding(), Server_StopHolding());
}

bool ULimenHoldComponent::IsHoldingSomething() const
{
	return WeaponData.Current.Get() != nullptr;
}

AActor* ULimenHoldComponent::GetActor() const
{
	return WeaponData.Current.Get();
}

void ULimenHoldComponent::OnRep_WeaponData(const FLimenItemHoldComponent_ReplicatedData& OldWeaponData)
{
	if (WeaponData.Current != OldWeaponData.Current)
	{
		OnItemChanged.Broadcast(WeaponData.Previous.Get(), WeaponData.Current.Get());
	}
}

void ULimenHoldComponent::Server_Hold_Implementation(AActor* InItem)
{
	Agnostic_Hold(InItem);
}

void ULimenHoldComponent::Server_StopHolding_Implementation()
{
	Agnostic_StopHolding();
}

void ULimenHoldComponent::Agnostic_Hold(AActor* InItem)
{
	if (!InItem || InItem == WeaponData.Current.Get())
	{
		if (GetOwner() && GetOwner()->HasAuthority())
		{
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
		}
		return;
	}
	
	WeaponData.Previous = WeaponData.Current.Get();
	WeaponData.Current = InItem;
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
	}

	OnItemChanged.Broadcast(WeaponData.Previous.Get(), WeaponData.Current.Get());
}

void ULimenHoldComponent::Agnostic_StopHolding()
{
	WeaponData.Previous = WeaponData.Current.Get();
	WeaponData.Current = nullptr;
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, WeaponData, this)
	}

	OnItemChanged.Broadcast(WeaponData.Previous.Get(), WeaponData.Current.Get());
}
