// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageItem.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Templates/SubclassOf.h"
#include "LimenStorageSaveData.h"
#include "LimenStorageSubsystem.generated.h"


class ULimenSaveSubsystem;
class ULimenStorageSaveData;
class ULimenSettingsSaveData;


class LIMENSTORAGE_API FLimenStorageSubsystem
{
public:
	FLimenStorageSubsystem();
	virtual ~FLimenStorageSubsystem();
	
	virtual void InitializeStorage(ULimenSaveSubsystem* InSaveSubsystem);
	virtual void DeinitializeStorage();
	
	virtual TArray<FText> GetCategories() const;
	virtual TArray<FText> GetCategoriesInAlphabeticalOrder() const;
	virtual int32 GetItemCount() const;
	virtual bool IsStorageEmpty() const;
	
	virtual ULimenStorageItem* GetItemWithDisplayName(TSubclassOf<ULimenStorageItem> ItemClass, const FText& InDisplayName) const;
	virtual TArray<ULimenStorageItem*> GetAllItemsOfCategory(TSubclassOf<ULimenStorageItem> ItemClass, const FText& InCategory) const;

	template<typename ItemType = ULimenStorageItem>
	TArray<ItemType*> GetAllItemsOfCategory(const FText& InCategory) const
	{
		static_assert(std::is_base_of_v<ULimenStorageItem, ItemType>);
		
		TArray<ItemType*> CategorySettings;
		for (ItemType* Setting : GetItems<ItemType>())
		{
			if (Setting->GetCategory().CompareTo(InCategory) == 0)
			{
				CategorySettings.Push(Cast<ItemType>(Setting));
			}
		}
		return CategorySettings;
	}

	virtual void Save();
	virtual void Load();
	virtual bool HasSavedData() const;
	
	virtual TArray<ULimenStorageItem*> GetItems(TSubclassOf<ULimenStorageItem> Class) const;
	template<typename ItemType = ULimenStorageItem>
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
	void RemoveItem(ULimenStorageItem* NewItem);

	ULimenStorageSaveData* GetCurrentSaveData() const;
	ULimenStorageSaveData* GenerateNewSaveData();
	
	const TArray<TStrongObjectPtr<ULimenStorageItem>>& GetStorageItems() const;
	
private:
	TWeakObjectPtr<ULimenSaveSubsystem> SaveSubsystem;
	TArray<TStrongObjectPtr<ULimenStorageItem>> StorageItems;
	TStrongObjectPtr<ULimenStorageSaveData> CurrentSaveData;

	bool bHasSavedData;

	virtual void Save_Internal();
	virtual void Load_Internal();
};



UCLASS(Abstract)
class LIMENSTORAGE_API ULimenGameInstanceStorageSubsystem : public UGameInstanceSubsystem, public FLimenStorageSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};



UCLASS(Abstract)
class LIMENSTORAGE_API ULimenLocalPlayerStorageSubsystem : public ULocalPlayerSubsystem, public FLimenStorageSubsystem
{
	GENERATED_BODY()
	
public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
};
