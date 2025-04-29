// Copyright Face Software. All Rights Reserved.


#include "Components/LimenPhysicalItemHoldComponent.h"

#include "Items/LimenPhysicalItem.h"
#include "Net/UnrealNetwork.h"


ULimenPhysicalItemHoldComponent::ULimenPhysicalItemHoldComponent()
{
	bIsHoldingSomething = false;
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
	if (bIsHoldingSomething)
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

	PhysicalItem = nullptr;

	bIsHoldingSomething = false;
	OnItemChanged.Broadcast(Previous, nullptr);
}

bool ULimenPhysicalItemHoldComponent::IsHoldingSomething() const
{
	return bIsHoldingSomething;
}

ALimenPhysicalItem* ULimenPhysicalItemHoldComponent::GetPhysicalItem() const
{
	return PhysicalItem.Get();
}

void ULimenPhysicalItemHoldComponent::OnRep_PhysicalItem()
{
	if (PhysicalItem)
	{
		bIsHoldingSomething = true;
		OnItemChanged.Broadcast(PreviousPhysicalItem.Get(), PhysicalItem.Get());
	}
	else
	{
		bIsHoldingSomething = false;
		OnItemChanged.Broadcast(PreviousPhysicalItem.Get(), nullptr);
	}
}
