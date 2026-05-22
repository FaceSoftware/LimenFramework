// Copyright Face Software. All Rights Reserved.


#include "LimenStorageSubsystem.h"

#include "LimenStorageItem.h"
#include "LimenStorageSaveData.h"
#include "BlueprintLibraries/LimenString.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenSaveSubsystem.h"

FLimenStorageSubsystem::FLimenStorageSubsystem() 
	: bHasSavedData(false)
{
}

FLimenStorageSubsystem::~FLimenStorageSubsystem()
{
}

void FLimenStorageSubsystem::InitializeStorage(ULimenSaveSubsystem* InSaveSubsystem)
{
	SaveSubsystem = InSaveSubsystem;
	if (ensureAlways(SaveSubsystem.IsValid()))
	{
		auto* TempSaveData = Cast<ULimenStorageSaveData>(SaveSubsystem->LoadData(SaveDataName));
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
}

void FLimenStorageSubsystem::DeinitializeStorage()
{
	Save_Internal();
	CurrentSaveData.Reset();
	StorageItems.Empty();
}

TArray<FText> FLimenStorageSubsystem::GetCategories() const
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

TArray<FText> FLimenStorageSubsystem::GetCategoriesInAlphabeticalOrder() const
{
	TArray<FText> Categories = GetCategories();
	Categories.Sort(&ULimenString::CompareText);
	return Categories;
}

int32 FLimenStorageSubsystem::GetItemCount() const
{
	return StorageItems.Num();
}

bool FLimenStorageSubsystem::IsStorageEmpty() const
{
	return StorageItems.IsEmpty();
}

ULimenStorageItem* FLimenStorageSubsystem::GetItemWithDisplayName(TSubclassOf<ULimenStorageItem> ItemClass,
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

TArray<ULimenStorageItem*> FLimenStorageSubsystem::GetAllItemsOfCategory(TSubclassOf<ULimenStorageItem> ItemClass,
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

void FLimenStorageSubsystem::Save()
{
	Save_Internal();
}

void FLimenStorageSubsystem::Load()
{
	Load_Internal();
}

bool FLimenStorageSubsystem::HasSavedData() const
{
	return bHasSavedData;
}

TArray<ULimenStorageItem*> FLimenStorageSubsystem::GetItems(const TSubclassOf<ULimenStorageItem> Class) const
{
	TArray<ULimenStorageItem*> Out;
	Out.Reserve(StorageItems.Num());
	for (const TStrongObjectPtr<ULimenStorageItem>& Item : StorageItems)
	{
		if (Item->IsA(Class))
		{
			Out.Push(Item.Get());
		}
	}
	return Out;
}

ULimenStorageItem* FLimenStorageSubsystem::GetItem(const TSubclassOf<ULimenStorageItem>& Class) const
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

void FLimenStorageSubsystem::AddItem(ULimenStorageItem* NewItem)
{
	StorageItems.Push(TStrongObjectPtr(NewItem));
}

void FLimenStorageSubsystem::RemoveItem(ULimenStorageItem* NewItem)
{
	StorageItems.Remove(TStrongObjectPtr(NewItem));
}

ULimenStorageSaveData* FLimenStorageSubsystem::GetCurrentSaveData() const
{
	return CurrentSaveData.Get();
}

ULimenStorageSaveData* FLimenStorageSubsystem::GenerateNewSaveData()
{
	if (CurrentSaveData.IsValid())
	{
		CurrentSaveData.Reset();
	}

	auto* TempSaveData = NewObject<ULimenStorageSaveData>();
	CurrentSaveData = TStrongObjectPtr(TempSaveData);
	return CurrentSaveData.Get();
}

const TArray<TStrongObjectPtr<ULimenStorageItem>>& FLimenStorageSubsystem::GetStorageItems() const
{
	return StorageItems;
}

void FLimenStorageSubsystem::Save_Internal()
{
	GenerateNewSaveData();
	for (const TStrongObjectPtr<ULimenStorageItem>& Item : StorageItems)
	{
		if (Item->ShouldSaveData())
		{
			CurrentSaveData->AddObjectSaveData(Item.Get());
		}
	}

	if (SaveSubsystem.IsValid())
	{
		verify(SaveSubsystem->SaveData(CurrentSaveData.Get(), SaveDataName));
	}
}

void FLimenStorageSubsystem::Load_Internal()
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
		const FSoftClassPath& ArchiveClassPath = SaveData.GetObjectClass();
		const UClass* ItemClass = ArchiveClassPath.TryLoadClass<ULimenStorageItem>();
		if (ItemClass == nullptr)
		{
			continue;
		}

		const TStrongObjectPtr<ULimenStorageItem>* Item = StorageItems.FindByPredicate([this, &ItemClass] (const TStrongObjectPtr<ULimenStorageItem>& Test)
		{
			return Test->GetClass() == ItemClass && Test->ShouldLoadData();
		});
		
		if (Item != nullptr)
		{
			SaveData.LoadData(Item->Get());
		}
		/// If the item no longer exists don't do anything
		/// This way there's backwards compatibility between updates
	}
}


///
///
///


void ULimenGameInstanceStorageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeStorage(Collection.InitializeDependency<ULimenSaveSubsystem>());
}

void ULimenGameInstanceStorageSubsystem::Deinitialize()
{
	Super::Deinitialize();
	DeinitializeStorage();
}


///
///
///


void ULimenLocalPlayerStorageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	InitializeStorage(GetWorld()->GetGameInstance()->GetSubsystem<ULimenSaveSubsystem>());
}

void ULimenLocalPlayerStorageSubsystem::Deinitialize()
{
	Super::Deinitialize();
	DeinitializeStorage();
}
