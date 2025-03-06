// Copyright Face Software. All Rights Reserved.


#include "LimenStorageSubsystem.h"

#include "LimenStorageItem.h"
#include "LimenStorageSaveData.h"
#include "BlueprintLibraries/LimenString.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenSaveSubsystem.h"

void ULimenStorageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	Collection.InitializeDependency(ULimenSaveSubsystem::StaticClass());
	
	auto* SaveSystem = GetGameInstance()->GetSubsystem<ULimenSaveSubsystem>();
	auto* TempSaveData = Cast<ULimenStorageSaveData>(SaveSystem->LoadData(SaveDataName));
	CurrentSaveData = TStrongObjectPtr(TempSaveData);
	bHasSavedData = CurrentSaveData.IsValid();
	if (!bHasSavedData)
	{
		// No previous save, create an initial save
		Save_Internal();
	}
	else
	{
		Load_Internal();
	}
}

void ULimenStorageSubsystem::Deinitialize()
{
	Save_Internal();
	Super::Deinitialize();
}

TArray<FText> ULimenStorageSubsystem::GetCategories() const
{
	TArray<FText> Categories;
	for (const ULimenStorageItem* Setting : GetItems<ULimenStorageItem>())
	{
		bool bAlreadyAdded = false;
		for (const FText& Category : Categories)
		{
			if (Setting->GetCategory().CompareTo(Category) == 0)
			{
				bAlreadyAdded = true;
				break;
			}
		}

		if (!bAlreadyAdded)
		{
			Categories.Push(Setting->GetCategory());
		}
	}
	
	return Categories;
}

TArray<FText> ULimenStorageSubsystem::GetCategoriesInAlphabeticalOrder() const
{
	TArray<FText> Categories = GetCategories();
	Categories.Sort(&ULimenString::CompareText);
	return Categories;
}

ULimenStorageItem* ULimenStorageSubsystem::GetItemWithDisplayName(TSubclassOf<ULimenStorageItem> ItemClass,
																  const FText& InDisplayName) const
{
	for (ULimenStorageItem* Setting : GetItems<ULimenStorageItem>())
	{
		if (Setting->GetDisplayName().CompareTo(InDisplayName) == 0)
		{
			return Setting;
		}
	}
	return nullptr;
}

TArray<ULimenStorageItem*> ULimenStorageSubsystem::GetAllItemsOfCategory(TSubclassOf<ULimenStorageItem> ItemClass,
																		 const FText& InCategory) const
{
	TArray<ULimenStorageItem*> CategorySettings;
	for (ULimenStorageItem* Setting : GetItems<ULimenStorageItem>())
	{
		if (Setting->GetCategory().CompareTo(InCategory) == 0)
		{
			CategorySettings.Push(Setting);
		}
	}
	return CategorySettings;
}

void ULimenStorageSubsystem::Save()
{
	Save_Internal();
}

void ULimenStorageSubsystem::Load()
{
	Load_Internal();
}

bool ULimenStorageSubsystem::HasSavedData() const
{
	return bHasSavedData;
}

ULimenStorageItem* ULimenStorageSubsystem::GetItem(const TSubclassOf<ULimenStorageItem>& Class) const
{
	check(Class.Get() != nullptr);
	for (const TStrongObjectPtr<ULimenStorageItem>& Item : StorageItems)
	{
		if (Item->IsA(Class))
		{
			return Item.Get();
		}
	}

	return nullptr;
}

void ULimenStorageSubsystem::AddItem(ULimenStorageItem* NewItem)
{
	StorageItems.Push(TStrongObjectPtr(NewItem));
}

ULimenStorageSaveData* ULimenStorageSubsystem::GetCurrentSaveData() const
{
	return CurrentSaveData.Get();
}

ULimenStorageSaveData* ULimenStorageSubsystem::GenerateNewSaveData()
{
	if (CurrentSaveData.IsValid())
	{
		CurrentSaveData.Reset();
	}

	auto* TempSaveData = NewObject<ULimenStorageSaveData>(this);
	CurrentSaveData = TStrongObjectPtr(TempSaveData);
	return CurrentSaveData.Get();
}

const TArray<TStrongObjectPtr<ULimenStorageItem>>& ULimenStorageSubsystem::GetStorageItems() const
{
	return StorageItems;
}

void ULimenStorageSubsystem::Save_Internal()
{
	GenerateNewSaveData();
	for (const TStrongObjectPtr<ULimenStorageItem>& Item : StorageItems)
	{
		if (Item->ShouldSaveData())
		{
			CurrentSaveData->AddObjectSaveData(Item.Get());
		}
	}

	auto* SaveSystem = GetGameInstance()->GetSubsystem<ULimenSaveSubsystem>();
	verify(SaveSystem->SaveData(CurrentSaveData.Get(), SaveDataName));
}

void ULimenStorageSubsystem::Load_Internal()
{
	const uint32 NumberOfItems = CurrentSaveData->GetObjectSaveDataCount();
	for (uint32 i = 0; i < NumberOfItems; ++i)
	{
		FObjectSaveData SaveData;
		if (!CurrentSaveData->GetObjectSaveData(i, SaveData))
		{
			continue;
		}

		// Search the items for the class
		const TSoftClassPtr<>& ItemClassSoftPtr = SaveData.GetObjectClass();
		if (ItemClassSoftPtr.IsNull())
		{
			continue;
		}
		
		const UClass* ItemClass = ItemClassSoftPtr.LoadSynchronous();
		const TStrongObjectPtr<ULimenStorageItem>* Item = StorageItems.FindByPredicate([this, &ItemClass] (const TStrongObjectPtr<ULimenStorageItem>& Test)
		{
			return Test->GetClass() == ItemClass && Test->ShouldLoadData();
		});
		
		if (Item != nullptr)
		{
			SaveData.LoadData(Item->Get());
		}
		/// If the items no longer exists don't do anything
		/// This way there's backwards compatibility between updates
	}
}