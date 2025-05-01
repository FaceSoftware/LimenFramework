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

	DOREPLIFETIME(ThisClass, PhysicalItem)
}

void ULimenPhysicalItemHoldComponent::Hold(ALimenPhysicalItem* InPhysicalItem)
{
	check(InPhysicalItem != nullptr);
	check(GetOwner()->HasAuthority())

	PreviousPhysicalItem = PhysicalItem.Get();
	if (PhysicalItem.IsValid())
	{
		StopHolding();
	}
	
	PhysicalItem = InPhysicalItem;
	OnRep_PhysicalItem();
}

void ULimenPhysicalItemHoldComponent::StopHolding()
{
	check(PhysicalItem != nullptr)
	check(GetOwner()->HasAuthority())

	ALimenPhysicalItem* Previous = PhysicalItem.Get();

	PhysicalItem.Reset();
	OnItemChanged.Broadcast(Previous, nullptr);
}

bool ULimenPhysicalItemHoldComponent::IsHoldingSomething() const
{
	return PhysicalItem.IsValid();
}

ALimenPhysicalItem* ULimenPhysicalItemHoldComponent::GetPhysicalItem() const
{
	return PhysicalItem.Get();
}

void ULimenPhysicalItemHoldComponent::OnRep_PhysicalItem()
{
	if (PhysicalItem.IsValid())
	{
		OnItemChanged.Broadcast(PreviousPhysicalItem.Get(), PhysicalItem.Get());
	}
	else
	{
		OnItemChanged.Broadcast(PreviousPhysicalItem.Get(), nullptr);
	}
}
