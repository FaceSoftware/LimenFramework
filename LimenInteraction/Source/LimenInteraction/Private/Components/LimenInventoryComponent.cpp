// Copyright Face Software. All Rights Reserved.


#include "Components/LimenInventoryComponent.h"

#include "EngineUtils.h"


ULimenInventoryComponent::ULimenInventoryComponent()
{
	InventorySize = 0;
	InventoryLoad = 0;
	bUseStaticSize = false;
}

TArray<ALimenItemBase*> ULimenInventoryComponent::LoadInventory(const TArray<TSubclassOf<ALimenItemBase>>& NewInventoryToLoad)
{
	ItemRegistries.Empty(NewInventoryToLoad.Num());

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
			ALimenItemBase* TempItem = GetWorld()->SpawnActor<ALimenItemBase>(ItemClass);
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
	check(NewItem != nullptr);

	if (!HasCapacity())
	{
		return false;
	}
	
	if (IsFirstOfType(NewItem->GetClass()))
	{
		FItemRegistry Registry;
		Registry.ItemClass = NewItem->GetClass();
		Registry.ItemInstances.Push(NewItem);
		ItemRegistries.Push(Registry);
		OnItemAdded.Broadcast(NewItem->GetClass());
	}
	else
	{
		FItemRegistry* Registry = FindItemRegistry(NewItem->GetClass());
		Registry->ItemInstances.Push(NewItem);
		OnItemUpdated.Broadcast(NewItem->GetClass());
	}
	
	OnInventoryUpdated.Broadcast(this);
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
		
		if (Registry->ItemInstances.IsEmpty())
		{
			break;
		}
	}

	if (Registry->ItemInstances.IsEmpty())
	{
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
		if (Registry.ItemClass.LoadSynchronous()->IsChildOf(Class))
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
		if (Registry.ItemClass.LoadSynchronous()->IsChildOf(Class) || Class == Registry.ItemClass.LoadSynchronous())
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
		if (Registry.ItemClass.LoadSynchronous() == ItemClass)
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

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		if (Registry.ItemClass.LoadSynchronous() == ItemClass)
		{
			return Registry.ItemInstances.Num();
		}
	}

	return 0;
}

bool ULimenInventoryComponent::HasCapacity() const
{
	return bUseStaticSize && InventoryLoad < InventorySize || !bUseStaticSize;
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
	InventoryLoad = Count;
}

FItemRegistry* ULimenInventoryComponent::FindItemRegistry(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	for (FItemRegistry& Entry : ItemRegistries)
	{
		if (ItemClass == Entry.ItemClass.LoadSynchronous() ||
			Entry.ItemClass.LoadSynchronous()->IsChildOf(ItemClass.Get()))
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
