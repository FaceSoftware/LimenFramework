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

void ULimenSlotInventoryComponent::AddSlot(const FName SlotName)
{
	Server_AddSlot(SlotName);
}

void ULimenSlotInventoryComponent::RemoveSlot(const FName SlotName)
{	
	Server_RemoveSlot(SlotName);
}

bool ULimenSlotInventoryComponent::AddItem(const FName& SlotName, const TScriptInterface<ILimenSlotInventoryItem>& Item)
{
	if (!Item) { return false; }
	if (!Item->CompatibleSlots().Contains(SlotName)) { return false; }
	
	FLimenReplicatedInventorySlot* Slot = ReplicatedSlots.Items.FindByKey(SlotName);
	if (!Slot) { return false; }
	if (Slot->Item != nullptr) { return false; }

	Slot->Item = Item;

	SlotUpdateInternal(SlotName, Item);
	OnSlotItemAdded.Broadcast(Slot->SlotName, Item);
	return true;
}

bool ULimenSlotInventoryComponent::GetItem(const TScriptInterface<ILimenSlotInventoryItem>& Item)
{
	if (!Item) { return false; }
	FLimenReplicatedInventorySlot* Slot = ReplicatedSlots.Items.FindByKey(Item);
	if (!Slot) { return false; }
	const TScriptInterface TempItem = Slot->Item;
	Slot->Item = nullptr;

	SlotUpdateInternal(Slot->SlotName, Item);
	OnSlotItemRemoved.Broadcast(Slot->SlotName, TempItem);
	return true;
}

TScriptInterface<ILimenSlotInventoryItem> ULimenSlotInventoryComponent::GetItemFromSlot(const FName SlotName)
{
	FLimenReplicatedInventorySlot* Slot = ReplicatedSlots.Items.FindByKey(SlotName);
	if (!Slot) { return nullptr; }
	if (!Slot->Item) { return nullptr; }
	
	TScriptInterface<ILimenSlotInventoryItem> TempItem = Slot->Item;
	Slot->Item = nullptr;

	SlotUpdateInternal(Slot->SlotName, nullptr);
	OnSlotItemRemoved.Broadcast(SlotName, TempItem);
	return TempItem;
}

TScriptInterface<ILimenSlotInventoryItem> ULimenSlotInventoryComponent::PeekItemFromSlot(const FName SlotName) const
{
	const FLimenReplicatedInventorySlot* Slot = ReplicatedSlots.Items.FindByKey(SlotName);
	if (!Slot) { return nullptr; }

	return Slot->Item;
}

void ULimenSlotInventoryComponent::SlotUpdateInternal(const FName& SlotName,
	const TScriptInterface<ILimenSlotInventoryItem>& Item)
{
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		FLimenReplicatedInventorySlot* SlotPtr = ReplicatedSlots.Items.FindByKey(SlotName);
		check(SlotPtr) // It shouldn't update a non-existent slot, if this hits, something is wrong elsewhere

		ReplicatedSlots.MarkItemDirty(*SlotPtr);
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
	}
}

void ULimenSlotInventoryComponent::CreateSlots()
{
	if (!GetOwner() || !GetOwner()->HasAuthority()) { return; }

	ReplicatedSlots.Items.Empty(Slots.Num());
	ReplicatedSlots.Items.AddDefaulted(Slots.Num());
	for (int i = 0; i < Slots.Num(); ++i)
	{
		ReplicatedSlots.Items[i].SlotName = Slots[i].SlotName;
		ReplicatedSlots.MarkItemDirty(ReplicatedSlots.Items[i]);
	}

	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
}

void ULimenSlotInventoryComponent::Server_AddSlot_Implementation(const FName& SlotName)
{
	const int32 Index = ReplicatedSlots.Items.Add(FLimenReplicatedInventorySlot(SlotName));
	ReplicatedSlots.MarkItemDirty(ReplicatedSlots.Items[Index]);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
}

void ULimenSlotInventoryComponent::Server_RemoveSlot_Implementation(const FName SlotName)
{
	const int32 SlotIndex = ReplicatedSlots.Items.IndexOfByKey(SlotName);
	if (SlotIndex == INDEX_NONE) { return; }
	ReplicatedSlots.Items.RemoveAt(SlotIndex);

	ReplicatedSlots.MarkArrayDirty();
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedSlots, this)
}
