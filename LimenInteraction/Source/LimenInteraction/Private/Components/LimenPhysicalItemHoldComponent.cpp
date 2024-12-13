// Copyright Face Software. All Rights Reserved.


#include "Components/LimenPhysicalItemHoldComponent.h"

#include "Items/LimenPhysicalItem.h"
#include "LogMacros/LimenInteractionLogMacros.h"
#include "LogMacros/LimenLogMacros.h"


void ULimenPhysicalItemHoldComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsHoldingSomething())
	{
		StopHolding();
	}
}

void ULimenPhysicalItemHoldComponent::Hold(ALimenPhysicalItem* InPhysicalItem)
{
	check(InPhysicalItem != nullptr);

	ALimenPhysicalItem* OldItem = PhysicalItem.Get();
	if (bIsHoldingSomething)
	{
		StopHolding();
	}

	bIsHoldingSomething = true;
	PhysicalItem = InPhysicalItem;
	
	OnItemChanged.Broadcast(OldItem, PhysicalItem.Get());
}

void ULimenPhysicalItemHoldComponent::StopHolding()
{
	check(PhysicalItem != nullptr)
	
	OnItemChanged.Broadcast(PhysicalItem.Get(), nullptr);

	PhysicalItem = nullptr;
	bIsHoldingSomething = false;
}

bool ULimenPhysicalItemHoldComponent::IsHoldingSomething() const
{
	return bIsHoldingSomething;
}

ALimenPhysicalItem* ULimenPhysicalItemHoldComponent::GetPhysicalItem() const
{
	return PhysicalItem.Get();
}

TSubclassOf<ALimenPhysicalItem> ULimenPhysicalItemHoldComponent::GetPhysicalItemParentClass() const
{
	return PhysicalItemParentClass.LoadSynchronous();
}
