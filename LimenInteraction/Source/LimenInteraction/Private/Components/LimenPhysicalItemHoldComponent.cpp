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

	ALimenPhysicalItem* OldItem = PhysicalItem.Get();
	if (bIsHoldingSomething)
	{
		StopHolding();
	}

	if (GetOwner()->HasAuthority())
	{
		PhysicalItem = InPhysicalItem;
		PhysicalItem->AddToGameplay();
	}

	bIsHoldingSomething = true;
	PhysicalItem->SetActorEnableCollision(false);
	OnItemChanged.Broadcast(OldItem, PhysicalItem.Get());
}

void ULimenPhysicalItemHoldComponent::StopHolding()
{
	check(PhysicalItem != nullptr)
	
	if (GetOwner()->HasAuthority())
	{
		PhysicalItem->RemoveFromGameplay();
		PhysicalItem = nullptr;
	}

	bIsHoldingSomething = false;
	OnItemChanged.Broadcast(PhysicalItem.Get(), nullptr);
}

void ULimenPhysicalItemHoldComponent::Drop()
{
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
	if (PhysicalItem == nullptr)
	{
		StopHolding();
	}
	else
	{
		Hold(PhysicalItem.Get());
	}
}
