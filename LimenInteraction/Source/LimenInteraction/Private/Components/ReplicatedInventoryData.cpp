// Copyright FaceSoftware. All Rights Reserved.


#include "Components/ReplicatedInventoryData.h"

#include "Async/Async.h"
#include "Components/LimenInventoryComponent.h"


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



FLimenReplicatedInventorySlot::FLimenReplicatedInventorySlot()
{
}

FLimenReplicatedInventorySlot::FLimenReplicatedInventorySlot(const FName& InSlotName) : SlotName(InSlotName)
{
}

void FLimenReplicatedInventorySlot::PreReplicatedRemove(
	const FLimenReplicatedInventorySlotsArray& InArraySerializer)
{
}

void FLimenReplicatedInventorySlot::PostReplicatedAdd(
	const FLimenReplicatedInventorySlotsArray& InArraySerializer)
{
}

void FLimenReplicatedInventorySlot::PostReplicatedChange(
	const FLimenReplicatedInventorySlotsArray& InArraySerializer)
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
}

void FLimenReplicatedInventorySlotsArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
}

void FLimenReplicatedInventorySlotsArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
}
