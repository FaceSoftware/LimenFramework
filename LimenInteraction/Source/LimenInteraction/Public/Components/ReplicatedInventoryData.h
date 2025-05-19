// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Network/LimenNetworkUtils.h"
#include "ReplicatedInventoryData.generated.h"


struct FReplicatedItemRegistryArray;
class ALimenItemBase;
class ULimenInventoryComponent;

USTRUCT()
struct FReplicatedItemRegistry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()	
	TSoftClassPtr<ALimenItemBase> ItemClass;
	UPROPERTY()
	TWeakObjectPtr<ALimenItemBase> ItemInstance;

	explicit FReplicatedItemRegistry(ALimenItemBase* InItem);
	FReplicatedItemRegistry();

	/**
	 * Called right before deleting element during replication.
	 * 
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * 
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PreReplicatedRemove(const FReplicatedItemRegistryArray& InArraySerializer);
	/**
	 * Called after adding and serializing a new element
	 *
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * 
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PostReplicatedAdd(const FReplicatedItemRegistryArray& InArraySerializer);
	/**
	 * Called after updating an existing element with new data
	 *
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	 void PostReplicatedChange(const FReplicatedItemRegistryArray& InArraySerializer);

	/**
	 * Called when logging LogNetFastTArray (log or lower verbosity)
	 *
	 * NOTE: intentionally not virtual; invoked via templated code.
	 * @see FExampleItemEntry
	 */
	FString GetDebugString();
};


USTRUCT()
struct FReplicatedItemRegistryArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FReplicatedItemRegistry> Items;
	UPROPERTY()
	TWeakObjectPtr<ULimenInventoryComponent> Owner;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FastArrayDeltaSerialize<FReplicatedItemRegistry, FReplicatedItemRegistryArray>(
			Items, DeltaParms, *this);
	}

	/**
	 * Called before removing elements and after the elements themselves are notified.  The indices are valid for this function call only!
	 *
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);

	/**
	 * Called after adding all new elements and after the elements themselves are notified.  The indices are valid for this function call only!
	 *
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);

	/**
	 * Called after updating all existing elements with new data and after the elements themselves are notified. The indices are valid for this function call only!
	 *
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
};

DECLARE_STRUCT_OPS_TYPE_TRAITS(FReplicatedItemRegistryArray)

