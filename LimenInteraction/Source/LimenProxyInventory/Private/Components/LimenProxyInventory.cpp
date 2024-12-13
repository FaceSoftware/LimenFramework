// Copyright Face Software. All Rights Reserved.


#include "Components/LimenProxyInventory.h"

#include "Items/LimenItemBase.h"


ULimenProxyInventory::ULimenProxyInventory()
{
	InventorySize = 4;
}

void ULimenProxyInventory::BeginPlay()
{
	Super::BeginPlay();

	Items.Reserve(InventorySize);
}

void ULimenProxyInventory::AddItem(const TSubclassOf<ALimenItemBase>& Item)
{
	check(Item != nullptr);
	check(CanAddItem());
	
	Items.Push(Item);
}

void ULimenProxyInventory::RemoveItem(const int32 Index)
{
	check(Items.IsValidIndex(Index));
	Items[Index] = nullptr;
}

bool ULimenProxyInventory::CanAddItem() const
{
	return Items.Num() <= InventorySize;
}

TSubclassOf<ALimenItemBase> ULimenProxyInventory::GetItem(const int32 Index) const
{
	check(Items.IsValidIndex(Index));
	return Items[Index];
}

int32 ULimenProxyInventory::GetInventorySize() const
{
	check(InventorySize == Items.Num());
	return InventorySize;
}
