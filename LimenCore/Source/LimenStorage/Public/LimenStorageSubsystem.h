// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageItem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
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

	UFUNCTION(BlueprintCallable, Category="Limen|Storage Subsystem")
	void Save();
	UFUNCTION(BlueprintCallable, Category="Limen|Storage Subsystem")
	void Load();
	UFUNCTION(BlueprintCallable, Category="Limen|Storage Subsystem")
	bool HasSavedData() const;

	template<typename ItemType>
	TArray<ItemType*> GetItems() const
	{
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		
		TArray<ItemType*> Out;
		Out.Reserve(StorageItems.Num());
		for (const TStrongObjectPtr<ULimenStorageItem>& Item : StorageItems)
		{
			if (Item->IsA<ItemType>())
			{
				Out.Push(CastChecked<ItemType>(Item.Get()));
			}
		}
		return Out;
	}
	
	template<typename ItemType>
	ItemType* GetItem() const
	{
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		return GetItem<ItemType>(ItemType::StaticClass());
	}

	template<typename ItemType>
	ItemType* GetItem(const TSubclassOf<ULimenStorageItem>& Class) const
	{
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		return Cast<ItemType>(GetItem(Class));
	}
	
	ULimenStorageItem* GetItem(const TSubclassOf<ULimenStorageItem>& Class) const;

protected:
	FString SaveDataName;
	
	void AddItem(ULimenStorageItem* NewItem);

	ULimenStorageSaveData* GetCurrentSaveData() const;
	ULimenStorageSaveData* GenerateNewSaveData();
	
	const TArray<TStrongObjectPtr<ULimenStorageItem>>& GetStorageItems() const;
	
private:
	TArray<TStrongObjectPtr<ULimenStorageItem>> StorageItems;
	UPROPERTY()
	TObjectPtr<ULimenStorageSaveData> CurrentSaveData;

	bool bHasSavedData;

	virtual void Save_Internal();
	virtual void Load_Internal();
};
