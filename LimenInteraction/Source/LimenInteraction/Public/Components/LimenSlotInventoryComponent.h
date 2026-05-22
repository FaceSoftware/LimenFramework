// Copyright FaceSoftware. All rights reserved.

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
FORCEINLINE uint32 GetTypeHash(const FLimenInventorySlot& Slot)
{
	return GetTypeHash(Slot.SlotName);
}


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenSlotInventoryComponent : public UActorComponent, public ILimenInventory
{
	GENERATED_BODY()
	
	friend FLimenReplicatedInventorySlotsArray;

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FSlotUpdate, const FName& /* SlotName */, const TScriptInterface<ILimenSlotInventoryItem>& /* Item */)
	FSlotUpdate OnSlotItemUpdated;
	FSlotUpdate OnSlotCreated;
	FSlotUpdate OnSlotRemoved;
	
	ULimenSlotInventoryComponent();
	virtual void InitializeComponent() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual FInventoryUpdate& GetOnInventoryUpdated() override;
	virtual FInventoryUpdate& GetOnInventoryRefreshed() override;
	virtual FInventoryItemUpdate& GetOnItemFailedToAdd() override;
	virtual FInventoryItemUpdate& GetOnItemAdded() override;
	virtual FInventoryItemUpdate& GetOnItemRemoved() override;
	virtual FInventoryItemUpdate& GetOnItemUpdated() override;
	
	FLimenInventorySlot& FindOrAddSlot(const FName& SlotName);
	void RemoveSlot(const FName& SlotName);
	
	void DynamicAddSlot(const FName SlotName);
	void DynamicRemoveSlot(const FName SlotName);
	
	bool DoesSlotExist(const FName& SlotName) const;
	
	bool AddItem(const FName& SlotName, const TScriptInterface<ILimenSlotInventoryItem>& Item);
	bool GetItem(const TScriptInterface<ILimenSlotInventoryItem>& Item);
	TScriptInterface<ILimenSlotInventoryItem> GetItemFromSlot(const FName& SlotName);
	template<typename T>
	T* GetItemFromSlot(const FName& SlotName)
	{
		static_assert(TIsDerivedFrom<T, UObject>::Value);		
		return Cast<T>(GetItemFromSlot(SlotName));
	}
	template<typename T>
	TScriptInterface<T> GetItemInterfaceFromSlot(const FName& SlotName)
	{
		static_assert(TIsIInterface<T>::Value);		
		return TScriptInterface<T>(GetItemFromSlot(SlotName)).GetObject();
	}

	TScriptInterface<ILimenSlotInventoryItem> PeekItemFromSlot(FName SlotName) const;
	template<typename T>
	T* PeekItemFromSlot(const FName& SlotName)
	{
		static_assert(TIsDerivedFrom<T, UObject>::Value);		
		return Cast<T>(PeekItemFromSlot(SlotName).GetObject());
	}
	template<typename T>
	TScriptInterface<T> PeekItemInterfaceFromSlot(const FName& SlotName)
	{
		static_assert(TIsIInterface<T>::Value);		
		return TScriptInterface<T>(PeekItemFromSlot(SlotName).GetObject());
	}

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSet<FLimenInventorySlot> Slots;

private:
	UPROPERTY(Replicated)
	FLimenReplicatedInventorySlotsArray ReplicatedSlots;
	TSet<FLimenReplicatedInventorySlot> ReplicatedSlotsSet;
	
	void CreateSlots();
	
	void SlotAdded(const FName& SlotName);
	void SlotRemoved(const FName& SlotName);
	void SlotItemUpdated(const FName& SlotName, const TScriptInterface<ILimenSlotInventoryItem>& Item);
};
