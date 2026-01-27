// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Network/LimenNetworkUtils.h"
#include "ReplicatedInventoryData.generated.h"


struct FLimenReplicatedInventorySlotsArray;
class ILimenSlotInventoryItem;
class ULimenSlotInventoryComponent;
struct FLimenReplicatedInventoryItemRegistryArray;
class ALimenItemBase;
class ULimenInventoryComponent;


///
/// ULimenInventoryComponent
///

USTRUCT()
struct FLimenReplicatedInventoryItemRegistry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()	
	TSoftClassPtr<AActor> ItemClass;
	UPROPERTY()
	TWeakObjectPtr<AActor> ItemInstance;

	explicit FLimenReplicatedInventoryItemRegistry(AActor* InItem);
	FLimenReplicatedInventoryItemRegistry();

	/**
	 * Called right before deleting element during replication.
	 * 
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * 
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PreReplicatedRemove(const FLimenReplicatedInventoryItemRegistryArray& InArraySerializer);
	/**
	 * Called after adding and serializing a new element
	 *
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * 
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PostReplicatedAdd(const FLimenReplicatedInventoryItemRegistryArray& InArraySerializer);
	/**
	 * Called after updating an existing element with new data
	 *
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	 void PostReplicatedChange(const FLimenReplicatedInventoryItemRegistryArray& InArraySerializer);

	/**
	 * Called when logging LogNetFastTArray (log or lower verbosity)
	 *
	 * NOTE: intentionally not virtual; invoked via templated code.
	 * @see FExampleItemEntry
	 */
	FString GetDebugString();
};


USTRUCT()
struct FLimenReplicatedInventoryItemRegistryArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FLimenReplicatedInventoryItemRegistry> Items;
	UPROPERTY()
	TWeakObjectPtr<ULimenInventoryComponent> Owner;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FastArrayDeltaSerialize<FLimenReplicatedInventoryItemRegistry, FLimenReplicatedInventoryItemRegistryArray>(
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

DECLARE_STRUCT_OPS_TYPE_TRAITS(FLimenReplicatedInventoryItemRegistryArray)



///
/// ULimenSlotInventoryComponent
///

USTRUCT()
struct FLimenReplicatedInventorySlot : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	FName SlotName;
	UPROPERTY()
	TScriptInterface<ILimenSlotInventoryItem> Item;

	FLimenReplicatedInventorySlot();
	explicit FLimenReplicatedInventorySlot(const FName& InSlotName);

	/**
	 * Called right before deleting element during replication.
	 * 
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * 
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PreReplicatedRemove(const FLimenReplicatedInventorySlotsArray& InArraySerializer);
	/**
	 * Called after adding and serializing a new element
	 *
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * 
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	void PostReplicatedAdd(const FLimenReplicatedInventorySlotsArray& InArraySerializer);
	/**
	 * Called after updating an existing element with new data
	 *
	 * @param InArraySerializer	Array serializer that owns the item and has triggered the replication call
	 * NOTE: intentionally not virtual; invoked via templated code, @see FExampleItemEntry
	 */
	 void PostReplicatedChange(const FLimenReplicatedInventorySlotsArray& InArraySerializer);

	/**
	 * Called when logging LogNetFastTArray (log or lower verbosity)
	 *
	 * NOTE: intentionally not virtual; invoked via templated code.
	 * @see FExampleItemEntry
	 */
	FString GetDebugString();
	
	bool operator==(const FName& InSlotName) const;
	bool operator==(const FLimenReplicatedInventorySlot& Other) const;
	bool operator==(const TScriptInterface<ILimenSlotInventoryItem>& InItem) const;
};


USTRUCT()
struct FLimenReplicatedInventorySlotsArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FLimenReplicatedInventorySlot> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FastArrayDeltaSerialize<FLimenReplicatedInventorySlot, FLimenReplicatedInventorySlotsArray>(
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

DECLARE_STRUCT_OPS_TYPE_TRAITS(FLimenReplicatedInventorySlotsArray)

