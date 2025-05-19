// Copyright FaceSoftware. All Rights Reserved.


#include "Components/ReplicatedInventoryData.h"

#include "Async/Async.h"
#include "Components/LimenInventoryComponent.h"


FReplicatedItemRegistry::FReplicatedItemRegistry(ALimenItemBase* InItem)
{
	check(InItem != nullptr);

	ItemClass = InItem->GetClass();
	ItemInstance = InItem;
}

FReplicatedItemRegistry::FReplicatedItemRegistry()
{
}

void FReplicatedItemRegistry::PreReplicatedRemove(const FReplicatedItemRegistryArray& InArraySerializer)
{
	AsyncTask(ENamedThreads::GameThread, [this, InArraySerializer]
	{
		if (!InArraySerializer.Owner.IsValid() || !InArraySerializer.Owner->GetOwner())
		{
			return;
		}

		if (!InArraySerializer.Owner->GetOwner()->HasAuthority())
		{
			InArraySerializer.Owner->RemoveItemInstance(ItemInstance.Get());
		}
	});
}

void FReplicatedItemRegistry::PostReplicatedAdd(const FReplicatedItemRegistryArray& InArraySerializer)
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

void FReplicatedItemRegistry::PostReplicatedChange(const FReplicatedItemRegistryArray& InArraySerializer)
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


FString FReplicatedItemRegistry::GetDebugString()
{
	return FString(TEXT(""));
}

void FReplicatedItemRegistryArray::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
}

void FReplicatedItemRegistryArray::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
}

void FReplicatedItemRegistryArray::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
}
