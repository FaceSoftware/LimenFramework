// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageItem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenStorageSubsystem.generated.h"

class ULimenStorageSaveData;
class ULimenSettingsSaveData;
class ULimenStorageItem;
/**
 * 
 */
UCLASS(Abstract)
class LIMENSTORAGE_API ULimenStorageSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	TArray<FText> GetCategories() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	TArray<FText> GetCategoriesInAlphabeticalOrder() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings", meta=(DeterminesOutputType="ItemClass"))
	ULimenStorageItem* GetItemWithDisplayName(TSubclassOf<ULimenStorageItem> ItemClass, const FText& InDisplayName) const;

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings", meta=(DeterminesOutputType="ItemClass"))
	TArray<ULimenStorageItem*> GetAllItemsOfCategory(TSubclassOf<ULimenStorageItem> ItemClass, const FText& InCategory) const;

	template<typename ItemType = ULimenStorageItem>
	TArray<ItemType*> GetAllItemsOfCategory(const FText& InCategory) const
	{
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		
		TArray<ULimenStorageItem*> CategorySettings;
		for (ItemType* Setting : GetItems<ItemType>())
		{
			if (Setting->GetCategory().CompareTo(InCategory) == 0)
			{
				CategorySettings.Push(Setting);
			}
		}
		return CategorySettings;
	}

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	void Save();
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	void Load();

	template<typename ItemType>
	TArray<ItemType*> GetItems() const
	{
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		
		TArray<ItemType*> Out;
		Out.Reserve(StorageItems.Num());
		for (auto* Item : StorageItems)
		{
			ItemType* Temp = Cast<ItemType>(Item);
			check(Temp != nullptr);
			Out.Push(Temp);
		}
		return Out;
	}
	
	template<typename ItemType>
	ItemType* GetItem() const
	{
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		
		for (ULimenStorageItem* Item : StorageItems)
		{
			ItemType* Test = Cast<ItemType>(Item);
			if (Test != nullptr)
			{
				return Test;
			}
		}

		return nullptr;
	}

	template<typename ItemType>
	ItemType* GetItem(const TSubclassOf<ULimenStorageItem>& Class) const
	{
		check(Class != nullptr);
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		
		for (ULimenStorageItem* Item : StorageItems)
		{
			if (Item->GetClass() == Class)
			{
				return Cast<ItemType>(Item);
			}
		}

		return nullptr;
	}
	
	ULimenStorageItem* GetItem(const TSubclassOf<ULimenStorageItem>& Class) const
	{
		check(Class != nullptr);
		for (ULimenStorageItem* Item : StorageItems)
		{
			if (Item->GetClass() == Class)
			{
				return Item;
			}
		}

		return nullptr;
	}

protected:
	FName SaveDataName;
	
	void AddItem(ULimenStorageItem* NewItem);

	ULimenStorageSaveData* GetCurrentSaveData() const;
	ULimenStorageSaveData* GenerateNewSaveData();
	
	const TArray<ULimenStorageItem*>& GetStorageItems() const;

	
	
private:
	UPROPERTY()
	TArray<ULimenStorageItem*> StorageItems;
	UPROPERTY()
	TObjectPtr<ULimenStorageSaveData> CurrentSaveData;

	virtual void Save_Internal();
	virtual void Load_Internal();
};
