// Copyright Face Software. All Rights Reserved.


#include "Components/LimenInventoryComponent.h"

#include "EngineUtils.h"
#include "Net/UnrealNetwork.h"


ULimenInventoryComponent::ULimenInventoryComponent()
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);

	InventorySize = 0;
	CurrentInventorySize = 0;
	CurrentInventoryLoad = 0;
	bLimitedSize = false;
}

void ULimenInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedItemRegistries,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentInventorySize,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

void ULimenInventoryComponent::BeginPlay()
{
	ReplicatedItemRegistries.Owner = this;
	CurrentInventorySize = InventorySize;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		for (auto& Item : StartingItems)
		{
			if (auto* ItemInst = GetWorld()->SpawnActor<AActor>(Item); ensureAlways(ItemInst))
			{
				AddItem(ItemInst);
			}
		}
	}

	Super::BeginPlay();
}

FInventoryUpdate& ULimenInventoryComponent::GetOnInventoryUpdated()
{
	return OnInventoryUpdated;
}

FInventoryUpdate& ULimenInventoryComponent::GetOnInventoryRefreshed()
{
	return OnInventoryRefreshed;
}

FInventoryItemUpdate& ULimenInventoryComponent::GetOnItemFailedToAdd()
{
	return OnItemFailedToAdd;
}

FInventoryItemUpdate& ULimenInventoryComponent::GetOnItemAdded()
{
	return OnItemAdded;
}

FInventoryItemUpdate& ULimenInventoryComponent::GetOnItemRemoved()
{
	return OnItemRemoved;
}

FInventoryItemUpdate& ULimenInventoryComponent::GetOnItemUpdated()
{
	return OnItemUpdated;
}

TArray<TScriptInterface<ILimenInventoryItem>> ULimenInventoryComponent::LoadInventory(
	const TArray<TSubclassOf<AActor>>& NewInventoryToLoad)
{
	ItemRegistries.Empty(NewInventoryToLoad.Num());

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	TArray<TScriptInterface<ILimenInventoryItem>> OutLoadedItems;
	for (const TSubclassOf<AActor>& ItemClass : NewInventoryToLoad)
	{
		check(ItemClass->ImplementsInterface(ULimenInventoryItem::StaticClass()));
		
		bool bWasLoaded = false;
		AActor* ItemPtr = nullptr;
		for (TActorIterator<AActor> It(GetWorld()); It; ++It)
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
			AActor* TempItem = GetWorld()->SpawnActor<AActor>(ItemClass, Params);
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

TArray<TScriptInterface<ILimenInventoryItem>> ULimenInventoryComponent::SaveInventory() const
{
	TArray<TScriptInterface<ILimenInventoryItem>> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	for (const TScriptInterface<ILimenInventoryItem>& Item : Registry.ItemInstances)
	{
		Out.Push(Item);
	}

	return Out;
}

bool ULimenInventoryComponent::IsEmpty() const
{
	return CurrentInventoryLoad <= 0;
}

bool ULimenInventoryComponent::AddItem(const TScriptInterface<ILimenInventoryItem>& NewItem)
{
	if (!CanAddItem(NewItem))
	{
		return false;
	}
	
	TArray<TScriptInterface<ILimenInventoryItem>> Instances = SpawnItemInstances(NewItem);
	Instances.Push(NewItem);

	if (IsFirstOfType(NewItem.GetObject()->GetClass()))
	{
		FItemRegistry Registry;
		Registry.ItemClass = NewItem.GetObject()->GetClass();
		Registry.ItemInstances.Append(MoveTemp(Instances));
		ItemRegistries.Push(MoveTemp(Registry));
		
		for (const auto& Item : Instances)
		{
			OnItemAdded.Broadcast(Item);
		}
	}
	else
	{
		FItemRegistry* Registry = FindItemRegistry(NewItem.GetObject()->GetClass());
		Registry->ItemInstances.Append(MoveTemp(Instances));
		
		for (const auto& Item : Instances)
		{
			OnItemUpdated.Broadcast(Item);
		}
	}
	UpdateInventoryLoad();

	ItemAdded(NewItem);
	OnInventoryUpdated.Broadcast(this);
	return true;
}

bool ULimenInventoryComponent::CanAddItem(const TScriptInterface<ILimenInventoryItem>& NewItem) const
{
	if (!NewItem) return false;

	const FItemRegistry* Registry = ItemRegistries.FindByPredicate([&NewItem] (const FItemRegistry& Registry)
	{
		return Registry.ItemClass == NewItem.GetObject()->GetClass();
	});

	if (Registry && Registry->ItemInstances.Find(NewItem) != INDEX_NONE) return false;
	if (const int32 ItemQuantity = NewItem->GetItemQuantity(); !HasCapacity(ItemQuantity)) return false;

	return true;
}

bool ULimenInventoryComponent::GetItemInstance(const TScriptInterface<ILimenInventoryItem>& Instance)
{
	if (!Instance) return true;

	FItemRegistry* Registry = FindItemRegistry(Instance.GetObject()->GetClass());
	if (Registry == nullptr) return false;

	bool bFound = false;
	for (int i = 0; i < Registry->ItemInstances.Num(); ++i)
	{
		if (Registry->ItemInstances[i] == Instance)
		{
			Registry->ItemInstances.RemoveAt(i);
			ItemRemoved(Instance);
			bFound = true;
			break;
		}
	}
	UpdateInventoryLoad();

	if (!bFound) return false;

	if (Registry->ItemInstances.IsEmpty())
	{
		ItemRegistries.RemoveAll([this, &Registry] (const FItemRegistry& Test)
		{
			return Test.ItemClass == Registry->ItemClass;
		});
		
		OnItemRemoved.Broadcast(Instance);
	}
	else
	{
		OnItemUpdated.Broadcast(Instance);
	}
	
	OnInventoryUpdated.Broadcast(this);
	return true;
}

TArray<TScriptInterface<ILimenInventoryItem>> ULimenInventoryComponent::GetItems(TSubclassOf<AActor> Class, const int32 Count)
{
	check(Class != nullptr && Count > 0);

	FItemRegistry* Registry = FindItemRegistry(Class);
	if (Registry == nullptr) { return {}; }

	TArray<TScriptInterface<ILimenInventoryItem>> OutItems;
	for (int i = 0; i < Count; ++i)
	{
		if (!Registry->ItemInstances.IsValidIndex(i))
		{
			break;
		}

		TScriptInterface<ILimenInventoryItem> TempItem = Registry->ItemInstances[i];
		Registry->ItemInstances.RemoveAt(i);

		check(TempItem != nullptr);

		OutItems.Push(TempItem);
		GetItemInstance(TempItem);

		if (Registry->ItemInstances.IsEmpty())
		{
			break;
		}

		i--;
	}
	UpdateInventoryLoad();

	return OutItems;
}

TScriptInterface<ILimenInventoryItem> ULimenInventoryComponent::GetItem(TSubclassOf<AActor> Class)
{
	TArray<TScriptInterface<ILimenInventoryItem>> Instances = GetItems(Class, 1);
	if (!Instances.IsValidIndex(0)) { return nullptr; }
	
	return Instances[0];
}

bool ULimenInventoryComponent::ContainsItem(const TSubclassOf<AActor>& Class) const
{
	if (!Class) return false;
	return PeekItemInstance(Class) != nullptr;
}

bool ULimenInventoryComponent::ContainsInstance(const TScriptInterface<ILimenInventoryItem>& Instance) const
{
	const FItemRegistry* Registry = FindItemRegistry(Instance.GetObject()->GetClass());
	if (!Registry) return false;
	return Registry->ItemInstances.Contains(Instance);
}

TMap<TSubclassOf<AActor>, int32> ULimenInventoryComponent::PeekInventory() const
{
	TMap<TSubclassOf<AActor>, int32> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		Out.Add(Registry.ItemClass.LoadSynchronous(), Registry.ItemInstances.Num());
	}

	return Out;
}

TArray<TScriptInterface<ILimenInventoryItem>> ULimenInventoryComponent::PeekInventoryInstances(const TSubclassOf<AActor> Class) const
{
	TArray<TScriptInterface<ILimenInventoryItem>> Out;
	for (const FItemRegistry& Registry : ItemRegistries)
	{
		const UClass* ClassPtr = Registry.ItemClass.LoadSynchronous();
		if (ClassPtr == Class || ClassPtr->IsChildOf(Class))
		{
			for (const TScriptInterface<ILimenInventoryItem>& Item : Registry.ItemInstances)
			{
				Out.Push(Item);
			}
		}
	}
	
	return Out;	
}

TScriptInterface<ILimenInventoryItem> ULimenInventoryComponent::PeekInventoryInstance(const TSubclassOf<AActor> Class) const
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

TMap<TSubclassOf<AActor>, int32> ULimenInventoryComponent::PeekItems() const
{
	TMap<TSubclassOf<AActor>, int32> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		Out.Add(Registry.ItemClass.LoadSynchronous(), Registry.ItemInstances.Num());
	}

	return Out;
}

TArray<TSubclassOf<AActor>> ULimenInventoryComponent::PeekItemsClass() const
{
	TArray<TSubclassOf<AActor>> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		Out.Add(Registry.ItemClass.LoadSynchronous());
	}

	return Out;
}

TMap<TSubclassOf<AActor>, int32> ULimenInventoryComponent::PeekItems(const TSubclassOf<AActor>& ItemClass) const
{
	if (ItemClass == nullptr)
    {
    	return TMap<TSubclassOf<AActor>, int32>();
    }

	TMap<TSubclassOf<AActor>, int32> Out;

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

TMap<TSubclassOf<AActor>, int32> ULimenInventoryComponent::PeekItems(const UClass* InterfaceClass) const
{
	if (InterfaceClass == nullptr)
	{
		return TMap<TSubclassOf<AActor>, int32>();
	}

	TMap<TSubclassOf<AActor>, int32> Out;

	for (const FItemRegistry& Registry : ItemRegistries)
	{
		if (Registry.ItemClass->ImplementsInterface(InterfaceClass))
		{
			Out.Add(Registry.ItemClass.LoadSynchronous(), Registry.ItemInstances.Num());
		}
	}

	return Out;
}

int32 ULimenInventoryComponent::GetItemQuantity(const TSubclassOf<AActor>& ItemClass) const
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
	return !bLimitedSize ||
		  ((CurrentInventoryLoad + ExtraDesiredSpace) <= CurrentInventorySize);
}

void ULimenInventoryComponent::ItemAdded(const TScriptInterface<ILimenInventoryItem>& NewItem)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ReplicateAddItem(NewItem);
	}
}

void ULimenInventoryComponent::ItemRemoved(const TScriptInterface<ILimenInventoryItem>& Item)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ReplicateRemoveItem(Item);
	}
}

void ULimenInventoryComponent::ReplicateAddItem(const TScriptInterface<ILimenInventoryItem>& NewItem)
{
	check(GetOwner() && GetOwner()->HasAuthority())

	const int32 Index = ReplicatedItemRegistries.Items.Add(FLimenReplicatedInventoryItemRegistry(NewItem->GetActor()));
	ReplicatedItemRegistries.MarkItemDirty(ReplicatedItemRegistries.Items[Index]);
}

void ULimenInventoryComponent::ReplicateRemoveItem(const TScriptInterface<ILimenInventoryItem>& NewItem)
{
	check(GetOwner() && GetOwner()->HasAuthority())

	const int32 Index = ReplicatedItemRegistries.Items.IndexOfByPredicate(
		[NewItem] (const FLimenReplicatedInventoryItemRegistry& Test)
		{
			return Test.ItemInstance == NewItem->GetActor();
		});

	ReplicatedItemRegistries.Items.RemoveAt(Index);
	ReplicatedItemRegistries.MarkArrayDirty();
}

void ULimenInventoryComponent::AddItemToRegistry(const TScriptInterface<ILimenInventoryItem>& NewItem)
{
	check(NewItem != nullptr);
	
	if (IsFirstOfType(NewItem.GetObject()->GetClass()))
	{
		FItemRegistry Registry;
		Registry.ItemClass = NewItem.GetObject()->GetClass();
		Registry.ItemInstances.Push(NewItem);
		
		ItemRegistries.Push(Registry);
	}
	else
	{
		FItemRegistry* Registry = FindItemRegistry(NewItem.GetObject()->GetClass());
		Registry->ItemInstances.Push(NewItem);
	}
}

void ULimenInventoryComponent::RemoveItemsFromRegistry(const TSubclassOf<AActor>& ItemToRemove, const int32 Count)
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

FItemRegistry* ULimenInventoryComponent::FindItemRegistry(const TSubclassOf<AActor>& ItemClass)
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

const FItemRegistry* ULimenInventoryComponent::FindItemRegistry(const TSubclassOf<AActor>& ItemClass) const
{
	check(ItemClass != nullptr);
	for (const FItemRegistry& Entry : ItemRegistries)
	{
		const UClass* ClassPtr = Entry.ItemClass.LoadSynchronous();
		if (ClassPtr == ItemClass || ClassPtr->IsChildOf(ItemClass))
		{
			return &Entry;
		}
	}

	return nullptr;
}

bool ULimenInventoryComponent::IsFirstOfType(const TSubclassOf<AActor>& ItemClass)
{
	const FItemRegistry* Registry = FindItemRegistry(ItemClass);
	if (Registry == nullptr)
	{
		return true;
	}

	return Registry->ItemInstances.IsEmpty();
}

TArray<TScriptInterface<ILimenInventoryItem>> ULimenInventoryComponent::SpawnItemInstances(const TScriptInterface<ILimenInventoryItem>& InItem) const
{
	TArray<TScriptInterface<ILimenInventoryItem>> Result;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.Template = InItem->GetActor();
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParameters.Owner = InItem->GetActor()->GetOwner();
	for (int i = 1; i < InItem->GetItemQuantity(); ++i)
	{
		AActor* Item = GetWorld()->SpawnActor<AActor>(InItem.GetObject()->GetClass(), SpawnParameters);
		Result.Push(Item);
	}

	return Result;
}
