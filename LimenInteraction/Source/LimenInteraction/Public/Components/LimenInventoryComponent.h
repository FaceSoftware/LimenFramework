// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenItemBase.h"
#include "LimenInventoryComponent.generated.h"


class ULimenInventoryComponent;


UENUM()
enum class EInventoryFeedback : uint8
{
	Unknown,
	Full,
	Success
};
USTRUCT()
struct FItemRegistry
{
	GENERATED_BODY();
	
	TSoftClassPtr<ALimenItemBase> ItemClass;

	UPROPERTY()
	TArray<ALimenItemBase*> ItemInstances;
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

	TArray<ALimenItemBase*> LoadInventory(const TArray<TSubclassOf<ALimenItemBase>>& NewInventoryToLoad);
	TArray<ALimenItemBase*> SaveInventory() const;

	/**
	 * @brief Adds a new item to the inventory.
	 * @param NewItem The instance of the new item.
	 * @return True if it could be added, false if it could not.
	 */
	bool AddItem(ALimenItemBase* NewItem);
	/**
	 * @brief Gets an item, removing it from the inventory.
	 * @return The item instance.
	 */
	template<typename T>
	T* GetItem()
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		const TSubclassOf<ALimenItemBase> Class = T::StaticClass();
		return Cast<T>(GetItem(Class));
	}
	/**
	 * @brief Gets an item, removing it from the inventory.
	 * @param Class The class of the item.
	 * @return The item instance.
	 */
	template<typename T>
	T* GetItem(const TSubclassOf<ALimenItemBase>& Class)
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		return Cast<T>(GetItem(Class));
	}
	/**
	 * @brief Gets an item, removing it from the inventory.
	 * @param Class The class of the item.
	 * @param Count The number of instances to get.
	 * @return The item instance.
	 */
	TArray<ALimenItemBase*> GetItem(const TSubclassOf<ALimenItemBase>& Class, const int32 Count);
	/**
	 * @brief Gets an item, removing it from the inventory.
	 * @param Class The class of the item.
	 * @return The item instance.
	 */
	ALimenItemBase* GetItem(const TSubclassOf<ALimenItemBase>& Class);
	/**
	 * @brief Gets all the inventory items, without removing them.
	 * @return A map containing the inventory item classes with their respective quantity.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	TMap<TSubclassOf<ALimenItemBase>, int32> PeekInventory() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory", meta=(DeterminesOutputType="Class"))
	TArray<ALimenItemBase*> PeekInventoryInstances(const TSubclassOf<ALimenItemBase> Class) const;
	UFUNCTION(BlueprintCallable, Category="Limen|Inventory", meta=(DeterminesOutputType="Class"))
	ALimenItemBase* PeekInventoryInstance(const TSubclassOf<ALimenItemBase> Class) const;

	TMap<TSubclassOf<ALimenItemBase>, int32> PeekItems() const;
	TArray<TSubclassOf<ALimenItemBase>> PeekItemsClass() const;
	/**
	 * @brief Gets inventory items from a specific class, without removing them.
	 * @param ItemClass The class to search.
	 * @return A map containing the inventory items of the specified class and their respective quantity.
	 */
	TMap<TSubclassOf<ALimenItemBase>, int32> PeekItems(const TSubclassOf<ALimenItemBase>& ItemClass) const;
	/**
	 * @brief Gets inventory items from a specific class, without removing them.
	 * @param InterfaceClass The interface class to search.
	 * @return A map containing the inventory items using the specified interface and their respective quantity.
	 */
	TMap<TSubclassOf<ALimenItemBase>, int32> PeekItems(const UClass* InterfaceClass) const;
	/**
	 * @brief Gets an instance from an item of the inventory, without removing it.
	 * @param ItemClass The item class to search.
	 * @return The item instance.
	 */
	template<typename T>
	T* PeekItemInstance(const TSubclassOf<ALimenItemBase>& ItemClass)
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		check(ItemClass != nullptr);
		for (FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemClass.LoadSynchronous() == ItemClass)
			{
				return Registry.ItemInstances.IsEmpty() ? nullptr : Cast<T>(Registry.ItemInstances[0]);
			}
		}
		return nullptr;
	}
	/**
	 * @brief Gets an instance from an item of the inventory, without removing it.
	 * @return The item instance.
	 */
	template<typename T>
	T* PeekItemInstance()
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		for (FItemRegistry& Registry : ItemRegistries)
		{
			T* Item = Registry.ItemInstances.IsEmpty() ? nullptr : Cast<T>(Registry.ItemInstances[0]);
			if (Item != nullptr)
			{
				return Item;
			}
		}
		return nullptr;
	}
	template<typename T>
	T* PeekItemInstance(const int32 Index)
	{
		static_assert(std::is_base_of_v<ALimenItemBase, T>);
		check(ItemRegistries.IsValidIndex(Index));
		
		T* Item = Cast<T>(ItemRegistries[Index].ItemInstances[0]);
		return Item;
	}
	template<typename T>
	TArray<T*> PeekItemInstances()
	{
		static_assert(TIsDerivedFrom<T, ALimenItemBase>::Value);
		
		TArray<T*> Out;
		Out.Reserve(ItemRegistries.Num());
		for (FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemInstances.IsEmpty() || !Registry.ItemClass->IsChildOf<T>() ||
				Registry.ItemClass == T::StaticClass())
			{
				continue;
			}

			for (ALimenItemBase* Instance : Registry.ItemInstances)
			{
				check(Instance != nullptr);
				
				T* Item = Cast<T>(Instance);
				if (Item != nullptr)
				{
					Out.Push(Item);
				}
			}
		}
		
		return Out;
	}
	template<typename InterfaceClass>
	TArray<ALimenItemBase*> PeekItemInstancesByInterface()
	{
		static_assert(TIsDerivedFrom<InterfaceClass, UInterface>::Value);
		
		TArray<ALimenItemBase*> Out;
		for (FItemRegistry& Registry : ItemRegistries)
		{
			if (Registry.ItemClass->ImplementsInterface(InterfaceClass::StaticClass()))
			{
				for (ALimenItemBase* Item : Registry.ItemInstances)
				{
					Out.Push(Item);
				}
			}
		}
		
		return Out;
	}

	UFUNCTION(BlueprintCallable, Category="Limen|Inventory")
	int32 GetItemQuantity(const TSubclassOf<ALimenItemBase>& ItemClass) const;

	bool HasCapacity() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Config", meta=(ClampMin="0", EditCondition="bUseStaticSize", EditConditionHides))
	uint16 InventorySize;
	UPROPERTY(EditDefaultsOnly, Category="Limen|Config")
	bool bUseStaticSize;
	
private:
	uint16 InventoryLoad;

	UPROPERTY()
	TArray<FItemRegistry> ItemRegistries;
	
	void AddItemToRegistry(ALimenItemBase* NewItem);
	void RemoveItemsFromRegistry(const TSubclassOf<ALimenItemBase>& ItemToRemove, const int32 Count);
	void UpdateInventoryLoad();
	FItemRegistry* FindItemRegistry(const TSubclassOf<ALimenItemBase>& ItemClass);
	
	bool IsFirstOfType(const TSubclassOf<ALimenItemBase>& ItemClass);
	
};
