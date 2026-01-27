// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ReplicatedInventoryData.h"
#include "Components/ActorComponent.h"
#include "Interfaces/LimenInventory.h"
#include "LimenSlotInventoryComponent.generated.h"


class ALimenItemBase;
class ILimenSlotInventoryItem;

USTRUCT(BlueprintType)
struct FLimenInventorySlot
{
	GENERATED_BODY()
	
	FLimenInventorySlot() = default;
	explicit FLimenInventorySlot(const FName& InSlotName)
	{
		SlotName = InSlotName;
	}
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SlotName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<UClass>> AllowedClasses;

	bool operator==(const FLimenInventorySlot& Other) const
	{
		return SlotName == Other.SlotName;
	}	
	bool operator==(const FName& InSlotName) const
	{
		return SlotName == InSlotName;
	}
};





UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenSlotInventoryComponent : public UActorComponent, public ILimenInventory
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FSlotUpdate, const FName& /* SlotName */, const TScriptInterface<ILimenSlotInventoryItem>& /* Item */)
	FSlotUpdate OnSlotItemAdded;
	FSlotUpdate OnSlotItemRemoved;
	
	ULimenSlotInventoryComponent();
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual FInventoryUpdate& GetOnInventoryUpdated() override;
	virtual FInventoryUpdate& GetOnInventoryRefreshed() override;
	virtual FInventoryItemUpdate& GetOnItemFailedToAdd() override;
	virtual FInventoryItemUpdate& GetOnItemAdded() override;
	virtual FInventoryItemUpdate& GetOnItemRemoved() override;
	virtual FInventoryItemUpdate& GetOnItemUpdated() override;
	
	void AddSlot(const FName SlotName);
	void RemoveSlot(const FName SlotName);
	
	bool AddItem(const FName& SlotName, const TScriptInterface<ILimenSlotInventoryItem>& Item);
	bool GetItem(const TScriptInterface<ILimenSlotInventoryItem>& Item);
	TScriptInterface<ILimenSlotInventoryItem> GetItemFromSlot(FName SlotName);
	TScriptInterface<ILimenSlotInventoryItem> PeekItemFromSlot(FName SlotName) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FLimenInventorySlot> Slots;

public:
	UPROPERTY(Replicated)
	FLimenReplicatedInventorySlotsArray ReplicatedSlots;
	
	void SlotUpdateInternal(const FName& SlotName, const TScriptInterface<ILimenSlotInventoryItem>& Item);
	void CreateSlots();
	
	UFUNCTION(Server, Reliable)
	void Server_AddSlot(const FName& SlotName);
	UFUNCTION(Server, Reliable)
	void Server_RemoveSlot(const FName SlotName);
};
