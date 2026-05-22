// Copyright FaceSoftware. All Rights Reserved.


#include "Components/ReplicatedInventoryData.h"

#include "Async/Async.h"
#include "Components/LimenInventoryComponent.h"
#include "Components/LimenSlotInventoryComponent.h"


FLimenReplicatedInventoryItemRegistry::FLimenReplicatedInventoryItemRegistry(AActor* InItem)
{
	check(InItem != nullptr);

	ItemClass = InItem->GetClass();
	ItemInstance = InItem;
}

FLimenReplicatedInventoryItemRegistry::FLimenReplicatedInventoryItemRegistry()
{
}

void FLimenReplicatedInventoryItemRegistry::PreReplicatedRemove(const FLimenReplicatedInventoryItemRegistryArray& InArraySerializer)
{
	AsyncTask(ENamedThreads::GameThread, [this, InArraySerializer]
	{
		if (!InArraySerializer.Owner.IsValid() || !InArraySerializer.Owner->GetOwner())
		{
			return;
		}

		if (!InArraySerializer.Owner->GetOwner()->HasAuthority())
		{
			InArraySerializer.Owner->GetItemInstance(ItemInstance.Get());
		}
	});
}

void FLimenReplicatedInventoryItemRegistry::PostReplicatedAdd(const FLimenReplicatedInventoryItemRegistryArray& InArraySerializer)
{
	AsyncTask(ENamedThreads::GameThread, [this, InArraySerializer]
	{
		if (!InArraySerializer.Owner.IsValid() || !InArraySerializer.Owner->GetOwner())
		{
			return;
		}

		if (!InArraySerializer.Owner->GetOwner()->HasAuthority())
		{
			if (ItemInstance.IsValid())
			{
				verify(InArraySerializer.Owner->AddItem(ItemInstance.Get()));
			}
		}
	});
}

void FLimenReplicatedInventoryItemRegistry::PostReplicatedChange(const FLimenReplicatedInventoryItemRegistryArray& InArraySerializer)
{
	AsyncTask(ENamedThreads::GameThread, [this, InArraySerializer]
	{
		if (!InArraySerializer.Owner.IsValid() || !InArraySerializer.Owner->GetOwner())
		{
			return;
		}

		if (!InArraySerializer.Owner->GetOwner()->HasAuthority())
		{
			if (ItemInstance.IsValid())
			{
				verify(InArraySerializer.Owner->AddItem(ItemInstance.Get()));
			}
		}
	});
}


FString FLimenReplicatedInventoryItemRegistry::GetDebugString()
{
	return FString(TEXT("FLimenReplicatedInventoryItemRegistry"));
}

void FLimenReplicatedInventoryItemRegistryArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
}

void FLimenReplicatedInventoryItemRegistryArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
}

void FLimenReplicatedInventoryItemRegistryArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
}

///
///
///

FLimenReplicatedInventorySlot::FLimenReplicatedInventorySlot()
{
}

FLimenReplicatedInventorySlot::FLimenReplicatedInventorySlot(const FName& InSlotName) : SlotName(InSlotName)
{
}

void FLimenReplicatedInventorySlot::PreReplicatedRemove(const FLimenReplicatedInventorySlotsArray& InArraySerializer)
{
}

void FLimenReplicatedInventorySlot::PostReplicatedAdd(const FLimenReplicatedInventorySlotsArray& InArraySerializer)
{
}

void FLimenReplicatedInventorySlot::PostReplicatedChange(const FLimenReplicatedInventorySlotsArray& InArraySerializer)
{
}

FString FLimenReplicatedInventorySlot::GetDebugString()
{
	return TEXT("FLimenReplicatedInventorySlot");
}

bool FLimenReplicatedInventorySlot::operator==(const FName& InSlotName) const
{
	return SlotName == InSlotName;
}

bool FLimenReplicatedInventorySlot::operator==(const FLimenReplicatedInventorySlot& Other) const
{
	return SlotName == Other.SlotName;
}

bool FLimenReplicatedInventorySlot::operator==(const TScriptInterface<ILimenSlotInventoryItem>& InItem) const
{
	return Item == InItem;
}

void FLimenReplicatedInventorySlotsArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (const int32 Idx : RemovedIndices)
	{
		const FName SlotName = Items[Idx].SlotName;
		const TWeakObjectPtr OwnerCopy = Owner;
		AsyncTask(ENamedThreads::GameThread, [OwnerCopy, SlotName]
		{
			if (!OwnerCopy.IsValid()) { return; }
			OwnerCopy->SlotRemoved(SlotName);
		});
	}
}

void FLimenReplicatedInventorySlotsArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (const int32 Idx : AddedIndices)
	{
		const FName SlotName = Items[Idx].SlotName;
		const TWeakObjectPtr OwnerCopy = Owner;
		AsyncTask(ENamedThreads::GameThread, [OwnerCopy, SlotName]
		{
			if (!OwnerCopy.IsValid()) { return; }
			OwnerCopy->SlotAdded(SlotName);
		});	
	}
}

void FLimenReplicatedInventorySlotsArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (const int32 Idx : ChangedIndices)
	{
		const FName SlotName = Items[Idx].SlotName;
		const TScriptInterface<ILimenSlotInventoryItem> Item = Items[Idx].Item;
		const TWeakObjectPtr OwnerCopy = Owner;
		AsyncTask(ENamedThreads::GameThread, [OwnerCopy, SlotName, Item]
		{
			if (!OwnerCopy.IsValid()) { return; }
			OwnerCopy->SlotItemUpdated(SlotName, Item);
		});	
	}
}
