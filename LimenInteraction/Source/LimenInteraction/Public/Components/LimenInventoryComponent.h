// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ReplicatedInventoryData.h"
#include "Async/Async.h"
#include "Interfaces/LimenInventory.h"
#include "Items/LimenItemBase.h"
#include "LimenInventoryComponent.generated.h"

class ULimenInventoryComponent;

USTRUCT()
struct FItemRegistry
{
	GENERATED_BODY();

	FItemRegistry() = default;
	FItemRegistry(const FItemRegistry& Other) = default;
	
	UPROPERTY()
	TSoftClassPtr<AActor> ItemClass;

	UPROPERTY()
	TArray<TScriptInterface<ILimenInventoryItem>> ItemInstances;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenInventoryComponent : public UActorComponent, public ILimenInventory
{
	GENERATED_BODY()

	friend FLimenReplicatedInventoryItemRegistryArray;

public:
	FInventoryUpdate OnInventoryUpdated;
	FInventoryUpdate OnInventoryRefreshed;
	FInventoryItemUpdate OnItemFailedToAdd;
	FInventoryItemUpdate OnItemAdded;
	FInventoryItemUpdate OnItemRemoved;
	FInventoryItemUpdate OnItemUpdated;
	
	ULimenInventoryComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	
	virtual FInventoryUpdate& GetOnInventoryUpdated() override;
	virtual FInventoryUpdate& GetOnInventoryRefreshed() override;
	virtual FInventoryItemUpdate& GetOnItemFailedToAdd() override;
	virtual FInventoryItemUpdate& GetOnItemAdded() override;
	virtual FInventoryItemUpdate& GetOnItemRemoved() override;
	virtual FInventoryItemUpdate& GetOnItemUpdated() override;

	/**
	 * @brief Loads the specified inventory items into the component.
	 * @param NewInventoryToLoad A list of item classes to load into the inventory.
	 * @return An array of item instances that have been successfully loaded.
	 */
	TArray<TScriptInterface<ILimenInventoryItem>> LoadInventory(const TArray<TSubclassOf<AActor>>& NewInventoryToLoad);
	/**
	 * @brief Saves the current inventory by collecting all item instances present in the component's registries.
	 * @return An array of item instances currently stored in the inventory.
	 */
	TArray<TScriptInterface<ILimenInventoryItem>> SaveInventory() const;

	UFUNCTION(BlueprintCallable)
	bool IsEmpty() const;
	/**
	 * @brief Adds a new item to the inventory.
	 * @param NewItem The item to add to the inventory. Must not be null.
	 * @return True if the item was successfully added, false otherwise.
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool AddItem(const TScriptInterface<ILimenInventoryItem>& NewItem);
	UFUNCTION(BlueprintCallable)
	virtual bool CanAddItem(const TScriptInterface<ILimenInventoryItem>& NewItem) const;

	/**
	 * @brief Removes a specific item instance from the inventory.
	 * @param Instance The item instance to be removed from the inventory.
	 */
	UFUNCTION(BlueprintCallable)
	bool GetItemInstance(const TScriptInterface<ILimenInventoryItem>& Instance);

	/**
	 * @brief Retrieves an item of the specified type from the inventory.
	 * @tparam T The type of the item to retrieve.
	 * @return A pointer to the item instance of the specified type. Returns nullptr if the item is not found.
	 */
	template<typename T>
	T* GetItem()
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value);
		UClass* ItemClass = T::StaticClass();
		if (!ItemClass->ImplementsInterface(ULimenInventoryItem::StaticClass())) { return nullptr; }

		return Cast<T>(GetItem(ItemClass).GetObject());
	}
	/**
	 * @brief Retrieves an item of the specified type from the inventory.
	 * @tparam T The type of the item to retrieve.
	 * @return A pointer to the item interface. Returns nullptr if the item is not found.
	 */
	template<typename T>
	TScriptInterface<ILimenInventoryItem> GetItemInterface()
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value);
		UClass* ItemClass = T::StaticClass();
		if (!ItemClass->ImplementsInterface(ULimenInventoryItem::StaticClass())) { return nullptr; }

		return GetItem(ItemClass);
	}
	template<typename T = AActor>
	TArray<T*> GetAllItems()
	{
		TArray<T*> Result;
		while (true)
		{
			if (T* Item = GetItem<T>())
			{
				Result.Push(Item);
			}
			else
			{
				break;
			}
		}

		return Result;
	}
	/**
	 * @brief Retrieves an item of the specified type from the inventory.
	 * @tparam T The type of the item to retrieve.
	 * @param Class The class of the item to search for.
	 * @return A pointer to the item instance of the specified type. Returns nullptr if the item is not found.
	 */
	template<typename T>
	T* GetItem(const TSubclassOf<AActor>& Class)
	{
		static_assert(std::is_base_of_v<AActor, T>);
		return Cast<T>(GetItem(Class));
	}
	/**
	 * @brief Retrieves a specified number of item instances of a given class from the inventory.
	 * @param Class The class type of the items to retrieve from the inventory.
	 * @param Count The number of instances to attempt to retrieve.
	 * @return An array with the retrieved item instances of the specified class.
	 * Returns an empty array if the class is not found or the count exceeds available items.
	 */
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType="Class"))
	TArray<TScriptInterface<ILimenInventoryItem>> GetItems(TSubclassOf<AActor> Class, const int32 Count);
	/**
	 * @brief Retrieves an item instance of the specified class from the inventory.
	 * @param Class The class type of the item to retrieve from the inventory.
	 * @return A pointer to the item instance of the specified class. Returns nullptr if the item is not found.
	 */
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType="Class"))
	TScriptInterface<ILimenInventoryItem> GetItem(TSubclassOf<AActor> Class);
	/**
	 * @brief Checks whether this inventory contains an item of the specified class.
	 * @param Class The class type of the item to check.
	 * @return True if an item of the class exists in the inventory, false otherwise or if class is invalid.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	bool ContainsItem(const TSubclassOf<AActor>& Class) const;
	/**
	 * @brief Checks whether this inventory contains the specified instance of an item.
	 * @param Instance Item instance to check.
	 * @return True if the instance is in the inventory, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	bool ContainsInstance(const TScriptInterface<ILimenInventoryItem>& Instance) const;
	/**
	 * @brief Retrieves a summary of inventory items currently stored in the component without modifying the inventory.
	 * @return A map where the keys represent item classes and the values indicate the respective quantities of
	 * each item in the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	TMap<TSubclassOf<AActor>, int32> PeekInventory() const;
	/**
	 * @brief Retrieves all inventory item instances that match the specified class or its subclasses without
	 * removing them.
	 * @param Class The class type to filter inventory items by, including subclasses of the specified class.
	 * @return An array of inventory item instances matching the specified class or its subclasses.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory", meta=(DeterminesOutputType="Class"))
	TArray<TScriptInterface<ILimenInventoryItem>> PeekInventoryInstances(const TSubclassOf<AActor> Class) const;
	/**
	 * @brief Retrieves the first inventory item instance matching the specified class type without removing them.
	 * @param Class The class type of the item to search for in the inventory.
	 * @return A pointer to the first item instance that matches the specified class type.
	 * Returns nullptr if no matching item is found.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory", meta=(DeterminesOutputType="Class"))
	TScriptInterface<ILimenInventoryItem> PeekInventoryInstance(const TSubclassOf<AActor> Class) const;

	/**
	 * @brief Provides a read-only view of the items currently available in the inventory.
	 * @return A map associating item classes with their corresponding counts in the inventory.
	 */
	TMap<TSubclassOf<AActor>, int32> PeekItems() const;
	/**
	 * @brief Retrieves a list of item classes currently stored in the inventory.
	 * @return An array of item classes that represent the types of items present in the inventory.
	 */
	TArray<TSubclassOf<AActor>> PeekItemsClass() const;
	/**
	 * @brief Retrieves a map of item classes and their corresponding counts available in the inventory that match or derive from the specified item class.
	 * @param ItemClass The class of the items to peek at. Can include derived classes.
	 * @return A map containing the matched item classes as keys and their respective counts as values.
	 */
	TMap<TSubclassOf<AActor>, int32> PeekItems(const TSubclassOf<AActor>& ItemClass) const;
	/**
	 * @brief Retrieves a mapping of item classes to their respective instance counts that
	 * implement the specified interface.
	 * @param InterfaceClass The interface class to filter the inventory items by.
	 * Only items implementing this interface will be included.
	 * @return A map where the keys are item classes that implement the specified interface*/
	TMap<TSubclassOf<AActor>, int32> PeekItems(const UClass* InterfaceClass) const;
	/**
	 * @brief Retrieves the first instance of a specific item class from the inventory without removing it.
	 * @param ItemClass The class type of the item to be retrieved.
	 * @return A pointer to the first item instance of the specified class, or nullptr if no such instance exists.
	 */
	template<typename T = AActor>
	T* PeekItemInstance(const TSubclassOf<AActor>& ItemClass) const
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value);
		check(ItemClass != nullptr);
		for (const FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemClass.LoadSynchronous() == ItemClass)
			{
				return Registry.ItemInstances.IsEmpty() ? nullptr : Cast<T>(Registry.ItemInstances[0].GetObject());
			}
		}
		return nullptr;
	}
	/**
	 * @brief Retrieves the first available item instance of the specified type from the inventory without removing it.
	 * @tparam T 
	 * @return A pointer to the item instance of the specified type if found, or nullptr if no such item exists.
	 */
	template<typename T>
	T* PeekItemInstance() const
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value);

		for (const FItemRegistry& Registry : ItemRegistries)
		{
			T* Item = Registry.ItemInstances.IsEmpty() ? nullptr : Cast<T>(Registry.ItemInstances[0].GetObject());
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
	template<typename T>
	T* PeekItemInstance(const int32 Index)
	{
		static_assert(std::is_base_of_v<AActor, T>);
		check(ItemRegistries.IsValidIndex(Index));
		
		T* Item = Cast<T>(ItemRegistries[Index].ItemInstances[0]);
		return Item;
	}
	/**
	 * @brief Retrieves an array with item instances of the specified type from the item registries without modifying their state.
	 * @tparam T The type of items to retrieve.
	 * @return An array with item instances of the specified type found in the registries.
	 */
	template<typename T>
	TArray<T*> PeekItemInstances() const
	{
		static_assert(TIsDerivedFrom<T, AActor>::Value);
		
		TArray<T*> Out;
		Out.Reserve(ItemRegistries.Num());
		for (const FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemInstances.IsEmpty() || !Registry.ItemClass->IsChildOf<T>() ||
				Registry.ItemClass == T::StaticClass())
			{
				continue;
			}

			for (const TScriptInterface<ILimenInventoryItem>& Instance : Registry.ItemInstances)
			{
				check(Instance != nullptr)				
				if constexpr (!std::is_same_v<T, AActor>)
				{
					T* Item = Cast<T>(Instance.GetObject());
					if (Item != nullptr)
					{
						Out.Push(Item);
					}
				}
				else
				{
					T* Item = Instance;
					check(Item != nullptr)
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
	TArray<TScriptInterface<ILimenInventoryItem>> PeekItemInstancesByInterface()
	{
		static_assert(TIsDerivedFrom<InterfaceClass, UInterface>::Value);
		
		TArray<TScriptInterface<ILimenInventoryItem>> Out;
		for (FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemClass->ImplementsInterface(InterfaceClass::StaticClass()))
			{
				for (const TScriptInterface<ILimenInventoryItem>& Item : Registry.ItemInstances)
				{
					Out.Push(Item);
				}
			}
		}
		
		return Out;
	}

	/**
	 * @brief Retrieves how many items of the specified class are stored in the inventory.
	 * @param ItemClass The class of the item for which the quantity is being requested.
	 * @return The total quantity of items of the specified class present in the inventory.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	int32 GetItemQuantity(const TSubclassOf<AActor>& ItemClass) const;

	/**
	 * @brief Checks if the inventory has the capacity to accommodate additional space.
	 * @param ExtraDesiredSpace The additional space required in the inventory.
	 * @return True if the inventory can accommodate the additional space, false otherwise.
	 */
	bool HasCapacity(const int32 ExtraDesiredSpace) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Invetory", meta=(ClampMin="0", EditCondition="bLimitedSize", EditConditionHides))
	uint16 InventorySize;
	UPROPERTY(EditDefaultsOnly, Category="Invetory")
	bool bLimitedSize;
	UPROPERTY(EditDefaultsOnly, Category="Invetory", meta=(MustImplement="/Script/LimenInteraction.LimenInventoryItem"))
	TArray<TSubclassOf<AActor>> StartingItems;

	virtual void ItemAdded(const TScriptInterface<ILimenInventoryItem>& NewItem);
	virtual void ItemRemoved(const TScriptInterface<ILimenInventoryItem>& Item);

	void ReplicateAddItem(const TScriptInterface<ILimenInventoryItem>& NewItem);
	void ReplicateRemoveItem(const TScriptInterface<ILimenInventoryItem>& NewItem);
	
private:
	uint16 CurrentInventoryLoad;
	UPROPERTY(Replicated, Transient)
	uint16 CurrentInventorySize;

	UPROPERTY(Transient)
	TArray<FItemRegistry> ItemRegistries;
	UPROPERTY(Replicated, Transient)
	FLimenReplicatedInventoryItemRegistryArray ReplicatedItemRegistries;
	
	void AddItemToRegistry(const TScriptInterface<ILimenInventoryItem>& NewItem);
	void RemoveItemsFromRegistry(const TSubclassOf<AActor>& ItemToRemove, const int32 Count);
	void UpdateInventoryLoad();
	FItemRegistry* FindItemRegistry(const TSubclassOf<AActor>& ItemClass);
	const FItemRegistry* FindItemRegistry(const TSubclassOf<AActor>& ItemClass) const;
	
	bool IsFirstOfType(const TSubclassOf<AActor>& ItemClass);
	TArray<TScriptInterface<ILimenInventoryItem>> SpawnItemInstances(const TScriptInterface<ILimenInventoryItem>& InItem) const;
	
};
