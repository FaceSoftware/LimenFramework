// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LimenSlotInventoryComponent.h"

#include "Interfaces/LimenSlotInventoryItem.h"
#include "Net/UnrealNetwork.h"


ULimenSlotInventoryComponent::ULimenSlotInventoryComponent()
{
	bWantsInitializeComponent = true;
}

void ULimenSlotInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	CreateSlots();
}

void ULimenSlotInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	constexpr FDoRepLifetimeParams Params = {
		COND_None,
		REPNOTIFY_OnChanged,
		true
	};

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedSlots, Params)
}

FInventoryUpdate& ULimenSlotInventoryComponent::GetOnInventoryUpdated()
{
	static FInventoryUpdate Temp;
	return Temp;
}

FInventoryUpdate& ULimenSlotInventoryComponent::GetOnInventoryRefreshed()
{
	static FInventoryUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenSlotInventoryComponent::GetOnItemFailedToAdd()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenSlotInventoryComponent::GetOnItemAdded()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenSlotInventoryComponent::GetOnItemRemoved()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenSlotInventoryComponent::GetOnItemUpdated()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

FLimenInventorySlot& ULimenSlotInventoryComponent::FindOrAddSlot(const FName& SlotName)
{
	check(!HasBegunPlay())
	return Slots.FindOrAdd(FLimenInventorySlot(SlotName));
}

void ULimenSlotInventoryComponent::RemoveSlot(const FName& SlotName)
{
	check(!HasBegunPlay())
	Slots.Remove(FLimenInventorySlot(SlotName));
}

void ULimenSlotInventoryComponent::DynamicAddSlot(const FName SlotName)
{
	check(HasBegunPlay())
	check(GetOwner())
	check(GetOwner()->HasAuthority())

	const int32 Index = ReplicatedSlots.Items.Add(FLimenReplicatedInventorySlot(SlotName));
	ReplicatedSlots.MarkItemDirty(ReplicatedSlots.Items[Index]);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
	
	SlotAdded(SlotName);
}

void ULimenSlotInventoryComponent::DynamicRemoveSlot(const FName SlotName)
{
	check(HasBegunPlay())
	check(GetOwner())
	check(GetOwner()->HasAuthority())
	
	const int32 SlotIndex = ReplicatedSlots.Items.IndexOfByKey(SlotName);
	if (SlotIndex == INDEX_NONE) { return; }
	ReplicatedSlots.Items.RemoveAt(SlotIndex);

	ReplicatedSlots.MarkArrayDirty();
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
	
	SlotRemoved(SlotName);
}

bool ULimenSlotInventoryComponent::DoesSlotExist(const FName& SlotName) const
{
	return ReplicatedSlotsSet.Contains(FLimenReplicatedInventorySlot(SlotName));
}

bool ULimenSlotInventoryComponent::AddItem(const FName& SlotName, const TScriptInterface<ILimenSlotInventoryItem>& Item)
{
	check(HasBegunPlay())
	check(GetOwner())
	check(GetOwner()->HasAuthority())

	if (!Item) { return false; }
	if (!Item->CompatibleSlots().Contains(SlotName)) { return false; }
	
	FLimenReplicatedInventorySlot* Slot = ReplicatedSlots.Items.FindByKey(SlotName);
	if (!Slot) { return false; }
	if (Slot->Item != nullptr) { return false; }

	Slot->Item = Item;
	
	ReplicatedSlots.MarkItemDirty(*Slot);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
	
	SlotItemUpdated(SlotName, Item);
	return true;
}

bool ULimenSlotInventoryComponent::GetItem(const TScriptInterface<ILimenSlotInventoryItem>& Item)
{
	check(HasBegunPlay())
	check(GetOwner())
	check(GetOwner()->HasAuthority())

	if (!Item) { return false; }
	FLimenReplicatedInventorySlot* Slot = ReplicatedSlots.Items.FindByKey(Item);
	if (!Slot) { return false; }
	const TScriptInterface TempItem = Slot->Item;
	Slot->Item = nullptr;

	ReplicatedSlots.MarkItemDirty(*Slot);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
	
	SlotItemUpdated(Slot->SlotName, nullptr);
	return true;
}

TScriptInterface<ILimenSlotInventoryItem> ULimenSlotInventoryComponent::GetItemFromSlot(const FName& SlotName)
{
	FLimenReplicatedInventorySlot* Slot = ReplicatedSlots.Items.FindByKey(SlotName);
	if (!Slot) { return nullptr; }
	if (!Slot->Item) { return nullptr; }
	
	TScriptInterface<ILimenSlotInventoryItem> TempItem = Slot->Item;
	Slot->Item = nullptr;
	
	ReplicatedSlots.MarkItemDirty(*Slot);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
	
	SlotItemUpdated(Slot->SlotName, nullptr);
	return TempItem;
}

TScriptInterface<ILimenSlotInventoryItem> ULimenSlotInventoryComponent::PeekItemFromSlot(const FName SlotName) const
{
	const FLimenReplicatedInventorySlot* Slot = ReplicatedSlotsSet.Find(FLimenReplicatedInventorySlot(SlotName));
	if (!Slot) { return nullptr; }

	return Slot->Item;
}

void ULimenSlotInventoryComponent::CreateSlots()
{
	ReplicatedSlots.Owner = this;

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ReplicatedSlots.Items.Empty(Slots.Num());
		ReplicatedSlots.Items.AddDefaulted(Slots.Num());
		int32 Idx = 0;
		for (const auto& Slot : Slots)
		{
			ReplicatedSlots.Items[Idx].SlotName = Slot.SlotName;
			ReplicatedSlots.MarkItemDirty(ReplicatedSlots.Items[Idx]);
			Idx++;
		}
	}
	
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
}

void ULimenSlotInventoryComponent::SlotAdded(const FName& SlotName)
{
	ReplicatedSlotsSet.Add(FLimenReplicatedInventorySlot(SlotName));
	OnSlotCreated.Broadcast(SlotName, nullptr);
}

void ULimenSlotInventoryComponent::SlotRemoved(const FName& SlotName)
{
	ReplicatedSlotsSet.Remove(FLimenReplicatedInventorySlot(SlotName));
	OnSlotRemoved.Broadcast(SlotName, nullptr);
}

void ULimenSlotInventoryComponent::SlotItemUpdated(const FName& SlotName,
	const TScriptInterface<ILimenSlotInventoryItem>& Item)
{
	FLimenReplicatedInventorySlot& Slot = *ReplicatedSlotsSet.Find(FLimenReplicatedInventorySlot(SlotName));
	Slot.Item = Item;
	
	OnSlotItemUpdated.Broadcast(Slot.SlotName, Item);
}
