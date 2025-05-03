// Copyright Face Software. All Rights Reserved.


#include "Components/LimenInventoryComponent.h"

#include "EngineUtils.h"


ULimenInventoryComponent::ULimenInventoryComponent()
{
	SetIsReplicatedByDefault(false);
	SetIsReplicated(false);

	InventorySize = 0;
	CurrentInventoryLoad = 0;
	bUseStaticSize = false;
}

TArray<ALimenItemBase*> ULimenInventoryComponent::LoadInventory(const TArray<TSubclassOf<ALimenItemBase>>& NewInventoryToLoad)
{
	ItemRegistries.Empty(NewInventoryToLoad.Num());

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<ALimenItemBase*> OutLoadedItems;
	for (const TSubclassOf<ALimenItemBase>& ItemClass : NewInventoryToLoad)
	{
		bool bWasLoaded = false;
		ALimenItemBase* ItemPtr = nullptr;
		for (TActorIterator<ALimenItemBase> It(GetWorld()); It; ++It)
		{
			if (It->GetClass() == ItemClass)
			{
				AddItemToRegistry(*It);
				bWasLoaded = true;
				ItemPtr = *It;
				break;
			}
		}

		if (!bWasLoaded)
		{
			ALimenItemBase* TempItem = GetWorld()->SpawnActor<ALimenItemBase>(ItemClass, Params);
			TempItem->RemoveFromGameplay();
			AddItemToRegistry(TempItem);
			ItemPtr = TempItem;
		}

		check(ItemPtr != nullptr);
		OutLoadedItems.Push(ItemPtr);
	}
	
	UpdateInventoryLoad();
	OnInventoryRefreshed.Broadcast(this);
	return OutLoadedItems;
}

TArray<ALimenItemBase*> ULimenInventoryComponent::SaveInventory() const
{
	TArray<ALimenItemBase*> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		for (ALimenItemBase* Item : Registry.ItemInstances)
		{
			Out.Push(Item);
		}
	}

	return Out;
}

bool ULimenInventoryComponent::AddItem(ALimenItemBase* NewItem)
{
	if (!CanAddItem(NewItem))
	{
		return false;
	}

	const bool bIsFirstOfType = IsFirstOfType(NewItem->GetClass());

	FItemRegistry* Registry;
	if (bIsFirstOfType)
	{
		Registry = new FItemRegistry();
		Registry->ItemClass = NewItem->GetClass();
	}
	else
	{
		Registry = FindItemRegistry(NewItem->GetClass());
	}

	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Template = NewItem;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = NewItem->GetOwner();
	for (int i = 1; i < NewItem->GetItemQuantity(); ++i)
	{
		ALimenItemBase* Item = GetWorld()->SpawnActor<ALimenItemBase>(NewItem->GetClass(), SpawnParameters);
		Item->RemoveFromGameplay();
		Registry->ItemInstances.Push(Item);
	}
	// Add the original item form the function args
	Registry->ItemInstances.Push(NewItem);

	if (bIsFirstOfType)
	{
		const FItemRegistry NewRegistry(*Registry);
		ItemRegistries.Push(NewRegistry);
		OnItemAdded.Broadcast(NewItem->GetClass());
		delete Registry;
	}
	else
	{
		OnItemUpdated.Broadcast(NewItem->GetClass());
	}

	ItemAdded(NewItem);
	OnInventoryUpdated.Broadcast(this);
	return true;
}

bool ULimenInventoryComponent::CanAddItem(ALimenItemBase* NewItem) const
{
	if (!NewItem)
	{
		return false;
	}

	const int32 ItemQuantity = NewItem->GetItemQuantity();
	if (!HasCapacity(ItemQuantity))
	{
		return false;
	}

	return true;
}

TArray<ALimenItemBase*> ULimenInventoryComponent::GetItem(const TSubclassOf<ALimenItemBase>& Class, const int32 Count)
{
	check(Class != nullptr && Count > 0);

	FItemRegistry* Registry = FindItemRegistry(Class);
	if (Registry == nullptr)
	{
		return TArray<ALimenItemBase*>();
	}

	TArray<ALimenItemBase*> OutItems;
	for (int i = 0; i < Count; ++i)
	{
		ALimenItemBase* TempItem = Registry->ItemInstances.Pop(EAllowShrinking::Yes);
		check(TempItem != nullptr);
		OutItems.Push(TempItem);
		ItemRemoved(TempItem);

		if (Registry->ItemInstances.IsEmpty())
		{
			break;
		}
	}

	if (Registry->ItemInstances.IsEmpty())
	{
		ItemRegistries.RemoveAll([this, &Registry] (const FItemRegistry& Test)
		{
			return Test.ItemClass == Registry->ItemClass;
		});
		
		OnItemRemoved.Broadcast(Registry->ItemClass.LoadSynchronous());
	}
	else
	{
		OnItemUpdated.Broadcast(Registry->ItemClass.LoadSynchronous());
	}
	
	OnInventoryUpdated.Broadcast(this);
	return OutItems;
}

ALimenItemBase* ULimenInventoryComponent::GetItem(const TSubclassOf<ALimenItemBase>& Class)
{
	TArray<ALimenItemBase*> Instances = GetItem(Class, 1);
	if (!Instances.IsValidIndex(0))
	{
		return nullptr;
	}

	return Instances[0];
}

TMap<TSubclassOf<ALimenItemBase>, int32> ULimenInventoryComponent::PeekInventory() const
{
	TMap<TSubclassOf<ALimenItemBase>, int32> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		Out.Add(Registry.ItemClass.LoadSynchronous(), Registry.ItemInstances.Num());
	}

	return Out;
}

TArray<ALimenItemBase*> ULimenInventoryComponent::PeekInventoryInstances(const TSubclassOf<ALimenItemBase> Class) const
{
	TArray<ALimenItemBase*> Out;
	for (const FItemRegistry& Registry : ItemRegistries)
	{
		const UClass* ClassPtr = Registry.ItemClass.LoadSynchronous();
		if (ClassPtr == Class || ClassPtr->IsChildOf(Class))
		{
			for (ALimenItemBase* Item : Registry.ItemInstances)
			{
				Out.Push(Item);
			}
		}
	}
	
	return Out;	
}

ALimenItemBase* ULimenInventoryComponent::PeekInventoryInstance(const TSubclassOf<ALimenItemBase> Class) const
{
	for (const FItemRegistry& Registry : ItemRegistries)
	{
		const UClass* ClassPtr = Registry.ItemClass.LoadSynchronous();
		if (ClassPtr == Class || ClassPtr->IsChildOf(Class))
		{
			return Registry.ItemInstances.IsEmpty() ? nullptr : Registry.ItemInstances[0];
		}
	}
	
	return nullptr;
}

TMap<TSubclassOf<ALimenItemBase>, int32> ULimenInventoryComponent::PeekItems() const
{
	TMap<TSubclassOf<ALimenItemBase>, int32> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		Out.Add(Registry.ItemClass.LoadSynchronous(), Registry.ItemInstances.Num());
	}

	return Out;
}

TArray<TSubclassOf<ALimenItemBase>> ULimenInventoryComponent::PeekItemsClass() const
{
	TArray<TSubclassOf<ALimenItemBase>> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		Out.Add(Registry.ItemClass.LoadSynchronous());
	}

	return Out;
}

TMap<TSubclassOf<ALimenItemBase>, int32> ULimenInventoryComponent::PeekItems(const TSubclassOf<ALimenItemBase>& ItemClass) const
{
	if (ItemClass == nullptr)
    {
    	return TMap<TSubclassOf<ALimenItemBase>, int32>();
    }

	TMap<TSubclassOf<ALimenItemBase>, int32> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		UClass* Class = Registry.ItemClass.LoadSynchronous();
		if (Class == ItemClass || Class->IsChildOf(ItemClass))
		{
			Out.Add(Registry.ItemClass.LoadSynchronous(), Registry.ItemInstances.Num());
		}
	}

	return Out;
}

TMap<TSubclassOf<ALimenItemBase>, int32> ULimenInventoryComponent::PeekItems(const UClass* InterfaceClass) const
{
	if (InterfaceClass == nullptr)
	{
		return TMap<TSubclassOf<ALimenItemBase>, int32>();
	}

	TMap<TSubclassOf<ALimenItemBase>, int32> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		if (Registry.ItemClass->ImplementsInterface(InterfaceClass))
		{
			Out.Add(Registry.ItemClass.LoadSynchronous(), Registry.ItemInstances.Num());
		}
	}

	return Out;
}

int32 ULimenInventoryComponent::GetItemQuantity(const TSubclassOf<ALimenItemBase>& ItemClass) const
{
	if (ItemClass == nullptr)
	{
		return 0;
	}

	int32 ItemQuantity = 0;
	for (const FItemRegistry& Registry : ItemRegistries)
	{
		const UClass* Class = Registry.ItemClass.LoadSynchronous();
		if (Class == ItemClass || Class->IsChildOf(ItemClass))
		{
			ItemQuantity += Registry.ItemInstances.Num();
		}
	}
	
	return ItemQuantity;
}

bool ULimenInventoryComponent::HasCapacity(const int32 ExtraDesiredSpace) const
{
	return !bUseStaticSize ||
		  ((CurrentInventoryLoad + ExtraDesiredSpace) <= InventorySize);
}

void ULimenInventoryComponent::ItemAdded(ALimenItemBase* NewItem)
{
}

void ULimenInventoryComponent::ItemRemoved(ALimenItemBase* NewItem)
{
}

void ULimenInventoryComponent::AddItemToRegistry(ALimenItemBase* NewItem)
{
	check(NewItem != nullptr);
	
	if (IsFirstOfType(NewItem->GetClass()))
	{
		FItemRegistry Registry;
		Registry.ItemClass = NewItem->GetClass();
		Registry.ItemInstances.Push(NewItem);
		
		ItemRegistries.Push(Registry);
	}
	else
	{
		FItemRegistry* Registry = FindItemRegistry(NewItem->GetClass());
		Registry->ItemInstances.Push(NewItem);
	}
}

void ULimenInventoryComponent::RemoveItemsFromRegistry(const TSubclassOf<ALimenItemBase>& ItemToRemove, const int32 Count)
{
	check(ItemToRemove)
	for (FItemRegistry& Registry : ItemRegistries)
	{
		if (Registry.ItemClass.LoadSynchronous() == ItemToRemove)
		{
			for (int i = 0; i < Count; ++i)
			{
				if (!Registry.ItemInstances.IsValidIndex(i))
				{
					return;
				}
				
				Registry.ItemInstances.RemoveAt(i, EAllowShrinking::Yes);
			}
		}
	}
}

void ULimenInventoryComponent::UpdateInventoryLoad()
{
	uint32 Count = 0;
	for (const auto& Entry : ItemRegistries)
	{
		Count += Entry.ItemInstances.Num();
	}
	CurrentInventoryLoad = Count;
}

FItemRegistry* ULimenInventoryComponent::FindItemRegistry(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	for (FItemRegistry& Entry : ItemRegistries)
	{
		const UClass* ClassPtr = Entry.ItemClass.LoadSynchronous();
		if (ClassPtr == ItemClass || ClassPtr->IsChildOf(ItemClass))
		{
			return &Entry;
		}
	}

	return nullptr;
}

bool ULimenInventoryComponent::IsFirstOfType(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	const FItemRegistry* Registry = FindItemRegistry(ItemClass);
	if (Registry == nullptr)
	{
		return true;
	}

	return Registry->ItemInstances.IsEmpty();
}
