// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenItemBase.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "Network/LimenNetworkUtils.h"
#include "LimenInventoryComponent.generated.h"

class ULimenInventoryComponent;
struct FLimenInventoryComponent_InventoryInstancesArray;


USTRUCT()
struct LIMENINTERACTION_API FLimenInventoryComponent_InventoryInstanceItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FLimenInventoryComponent_InventoryInstanceItem() = default;
	explicit FLimenInventoryComponent_InventoryInstanceItem(ALimenItemBase* Inst) : Instance(Inst)
	{
	}

	UPROPERTY()
	TWeakObjectPtr<ALimenItemBase> Instance;

	void PreReplicatedRemove(const FLimenInventoryComponent_InventoryInstancesArray& InArraySerializer) {}
	void PostReplicatedAdd(const FLimenInventoryComponent_InventoryInstancesArray& InArraySerializer) {}
	void PostReplicatedChange(const FLimenInventoryComponent_InventoryInstancesArray& InArraySerializer) {}
	FString GetDebugString() { return TEXT(""); }
};

USTRUCT()
struct LIMENINTERACTION_API FLimenInventoryComponent_InventoryInstancesArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FLimenInventoryComponent_InventoryInstancesArray() = default;

	UPROPERTY()
	TArray<FLimenInventoryComponent_InventoryInstanceItem> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms)
	{
		return FastArrayDeltaSerialize<FLimenInventoryComponent_InventoryInstanceItem, FLimenInventoryComponent_InventoryInstancesArray>(Items, DeltaParms,*this);
	}
};

DECLARE_STRUCT_OPS_TYPE_TRAITS(FLimenInventoryComponent_InventoryInstancesArray)

USTRUCT()
struct LIMENINTERACTION_API FItemRegistry
{
	GENERATED_BODY();
	
	TSoftClassPtr<ALimenItemBase> ItemClass;

	UPROPERTY()
	FLimenInventoryComponent_InventoryInstancesArray ItemInstances;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryItemUpdate, TSubclassOf<ALimenItemBase>, ItemClass);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInventoryUpdate, ULimenInventoryComponent*, Inventory);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FInventoryUpdate OnInventoryUpdated;
	UPROPERTY(BlueprintAssignable)
	FInventoryUpdate OnInventoryRefreshed;
	UPROPERTY(BlueprintAssignable)
	FInventoryItemUpdate OnItemFailedToAdd;
	UPROPERTY(BlueprintAssignable)
	FInventoryItemUpdate OnItemAdded;
	UPROPERTY(BlueprintAssignable)
	FInventoryItemUpdate OnItemRemoved;
	UPROPERTY(BlueprintAssignable)
	FInventoryItemUpdate OnItemUpdated;
	
	ULimenInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Loads the specified inventory items into the component.
	 * @param NewInventoryToLoad A list of item classes to load into the inventory.
	 * @return An array of item instances that have been successfully loaded.
	 */
	TArray<ALimenItemBase*> LoadInventory(const TArray<TSubclassOf<ALimenItemBase>>& NewInventoryToLoad);
	/**
	 * @brief Saves the current inventory by collecting all item instances present in the component's registries.
	 * @return An array of item instances currently stored in the inventory.
	 */
	TArray<ALimenItemBase*> SaveInventory() const;

	/**
	 * @brief Adds a new item to the inventory.
	 * @param NewItem The item to add to the inventory. Must not be null.
	 * @return True if the item was successfully added, false otherwise.
	 */
	virtual bool AddItem(ALimenItemBase* NewItem);
	virtual bool CanAddItem(ALimenItemBase* NewItem) const;
	TArray<ALimenItemBase*> GetAllItems();
	/**
	 * @brief Retrieves an item of the specified type from the inventory.
	 * @tparam T The type of the item to retrieve. Must derive from ALimenItemBase.
	 * @return A pointer to the item instance of the specified type. Returns nullptr if the item is not found.
	 */
	template<typename T>
	T* GetItem()
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		const TSubclassOf<ALimenItemBase> Class = T::StaticClass();
		return Cast<T>(GetItem(Class));
	}

	/**
	 * @brief Retrieves an item of the specified type from the inventory.
	 * @tparam T The type of the item to retrieve. Must derive from ALimenItemBase.
	 * @param Class The class of the item to search for.
	 * @return A pointer to the item instance of the specified type. Returns nullptr if the item is not found.
	 */
	template<typename T>
	T* GetItem(const TSubclassOf<ALimenItemBase>& Class)
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		return Cast<T>(GetItem(Class));
	}

	/**
	 * @brief Retrieves a specified number of item instances of a given class from the inventory.
	 * @param Class The class type of the items to retrieve from the inventory.
	 * @param Count The number of instances to attempt to retrieve.
	 * @return An array with the retrieved item instances of the specified class.
	 * Returns an empty array if the class is not found or the count exceeds available items.
	 */
	TArray<ALimenItemBase*> GetItem(const TSubclassOf<ALimenItemBase>& Class, const int32 Count);
	/**
	 * @brief Retrieves an item instance of the specified class from the inventory.
	 * @param Class The class type of the item to retrieve from the inventory.
	 * @return A pointer to the item instance of the specified class. Returns nullptr if the item is not found.
	 */
	ALimenItemBase* GetItem(const TSubclassOf<ALimenItemBase>& Class);
	/**
	 * @brief Retrieves a summary of inventory items currently stored in the component without modifying the inventory.
	 * @return A map where the keys represent item classes and the values indicate the respective quantities of
	 * each item in the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	TMap<TSubclassOf<ALimenItemBase>, int32> PeekInventory() const;
	/**
	 * @brief Retrieves all inventory item instances that match the specified class or its subclasses without
	 * removing them.
	 * @param Class The class type to filter inventory items by, including subclasses of the specified class.
	 * @return An array of inventory item instances matching the specified class or its subclasses.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory", meta=(DeterminesOutputType="Class"))
	TArray<ALimenItemBase*> PeekInventoryInstances(const TSubclassOf<ALimenItemBase> Class) const;
	/**
	 * @brief Retrieves the first inventory item instance matching the specified class type without removing them.
	 * @param Class The class type of the item to search for in the inventory.
	 * @return A pointer to the first item instance that matches the specified class type.
	 * Returns nullptr if no matching item is found.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory", meta=(DeterminesOutputType="Class"))
	ALimenItemBase* PeekInventoryInstance(const TSubclassOf<ALimenItemBase> Class) const;

	/**
	 * @brief Provides a read-only view of the items currently available in the inventory.
	 * @return A map associating item classes with their corresponding counts in the inventory.
	 */
	TMap<TSubclassOf<ALimenItemBase>, int32> PeekItems() const;
	/**
	 * @brief Retrieves a list of item classes currently stored in the inventory.
	 * @return An array of item classes that represent the types of items present in the inventory.
	 */
	TArray<TSubclassOf<ALimenItemBase>> PeekItemsClass() const;
	/**
	 * @brief Retrieves a map of item classes and their corresponding counts available in the inventory that match or derive from the specified item class.
	 * @param ItemClass The class of the items to peek at. Can include derived classes.
	 * @return A map containing the matched item classes as keys and their respective counts as values.
	 */
	TMap<TSubclassOf<ALimenItemBase>, int32> PeekItems(const TSubclassOf<ALimenItemBase>& ItemClass) const;
	/**
	 * @brief Retrieves a mapping of item classes to their respective instance counts that
	 * implement the specified interface.
	 * @param InterfaceClass The interface class to filter the inventory items by.
	 * Only items implementing this interface will be included.
	 * @return A map where the keys are item classes that implement the specified interface*/
	TMap<TSubclassOf<ALimenItemBase>, int32> PeekItems(const UClass* InterfaceClass) const;
	/**
	 * @brief Retrieves the first instance of a specific item class from the inventory without removing it.
	 * @param ItemClass The class type of the item to be retrieved.
	 * @return A pointer to the first item instance of the specified class, or nullptr if no such instance exists.
	 */
	template<typename T = ALimenItemBase>
	T* PeekItemInstance(const TSubclassOf<ALimenItemBase>& ItemClass) const
	{
		static_assert(TIsDerivedFrom<T, ALimenItemBase>::Value);
		check(ItemClass != nullptr);
		for (const FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemClass.LoadSynchronous() == ItemClass)
			{
				return Registry.ItemInstances.Items.IsEmpty() ? nullptr : Cast<T>(Registry.ItemInstances.Items[0].Instance.Get());
			}
		}
		return nullptr;
	}
	/**
	 * @brief Retrieves the first available item instance of the specified type from the inventory without removing it.
	 * @tparam T A class type that must derive from ALimenItemBase.
	 * @return A pointer to the item instance of the specified type if found, or nullptr if no such item exists.
	 */
	template<typename T = ALimenItemBase>
	T* PeekItemInstance() const
	{
		static_assert(TIsDerivedFrom<T, ALimenItemBase>::Value);

		for (const FItemRegistry& Registry : ItemRegistries)
		{
			T* Item = Registry.ItemInstances.Items.IsEmpty() ? nullptr : Cast<T>(Registry.ItemInstances.Items[0]);
			if (Item != nullptr)
			{
				return Item;
			}
		}
		return nullptr;
	}
	/**
	 * @brief Retrieves the first item instance of the specified type from the inventory at the given index without removing it.
	 * @param Index The index of the inventory slot to retrieve the item instance from.
	 * @return A pointer to the item instance if it exists and is of the specified type, otherwise nullptr.
	 */
	template<typename T = ALimenItemBase>
	T* PeekItemInstance(const int32 Index) const
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		check(ItemRegistries.IsValidIndex(Index));
		
		T* Item = Cast<T>(ItemRegistries[Index].ItemInstances.Items[0]);
		return Item;
	}
	/**
	 * @brief Retrieves an array with item instances of the specified type from the item registries without modifying their state.
	 * @tparam T The type of items to retrieve, which must derive from ALimenItemBase.
	 * @return An array with item instances of the specified type found in the registries.
	 */
	template<typename T = ALimenItemBase>
	TArray<T*> PeekItemInstances() const
	{
		static_assert(TIsDerivedFrom<T, ALimenItemBase>::Value);
		
		TArray<T*> Out;
		Out.Reserve(ItemRegistries.Num());
		for (const FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemInstances.Items.IsEmpty() || !Registry.ItemClass->IsChildOf<T>() ||
				Registry.ItemClass == T::StaticClass())
			{
				continue;
			}

			for (const FLimenInventoryComponent_InventoryInstanceItem& Instance : Registry.ItemInstances.Items)
			{
				check(Instance.Instance != nullptr);
				
				T* Item = Cast<T>(Instance.Instance);
				if (Item != nullptr)
				{
					Out.Push(Item);
				}
			}
		}
		
		return Out;
	}
	/**
	 * @brief Retrieves item instances from the inventory that implement a specific interface.
	 * @tparam InterfaceClass The class of the interface to filter item instances by.
	 * @return An array of item instances that implement the specified interface.
	 */
	template<typename InterfaceClass>
	TArray<ALimenItemBase*> PeekItemInstancesByInterface() const
	{
		static_assert(TIsDerivedFrom<InterfaceClass, UInterface>::Value);
		
		TArray<ALimenItemBase*> Out;
		for (const FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemClass->ImplementsInterface(InterfaceClass::StaticClass()))
			{
				for (const FLimenInventoryComponent_InventoryInstanceItem& Item : Registry.ItemInstances.Items)
				{
					Out.Push(Item.Instance.Get());
				}
			}
		}
		
		return Out;
	}

	/**
	 * @brief Retrieves the quantity of items of the specified class stored in the inventory.
	 * @param ItemClass The class of the item for which the quantity is being requested.
	 * @return The total quantity of items of the specified class present in the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	int32 GetItemQuantity(const TSubclassOf<ALimenItemBase>& ItemClass) const;

	/**
	 * @brief Checks if the inventory has the capacity to accommodate additional space.
	 * @param ExtraDesiredSpace The additional space required in the inventory.
	 * @return True if the inventory can accommodate the additional space, false otherwise.
	 */
	bool HasCapacity(const int32 ExtraDesiredSpace) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Config", meta=(ClampMin="0", EditCondition="bUseStaticSize", EditConditionHides))
	uint16 InventorySize;
	UPROPERTY(EditDefaultsOnly, Category="Limen|Config")
	bool bUseStaticSize;

	virtual void ItemAdded(ALimenItemBase* NewItem);
	virtual void ItemRemoved(ALimenItemBase* NewItem);
	
private:
	uint16 CurrentInventoryLoad;

	UPROPERTY(Replicated)
	TArray<FItemRegistry> ItemRegistries;
	
	void AddItemToRegistry(ALimenItemBase* NewItem);
	void RemoveItemsFromRegistry(const TSubclassOf<ALimenItemBase>& ItemToRemove, const int32 Count);
	void UpdateInventoryLoad();
	FItemRegistry* FindItemRegistry(const TSubclassOf<ALimenItemBase>& ItemClass);
	
	bool IsFirstOfType(const TSubclassOf<ALimenItemBase>& ItemClass);
};
