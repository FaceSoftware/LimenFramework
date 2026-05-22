// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAssets/LimenGridItemDatabase.h"
#include "Interfaces/LimenInventory.h"
#include "Items/LimenItemBase.h"
#include "LimenGridInventoryComponent.generated.h"


class ULimenGridItemDatabase;

struct FGridInventoryCell
{
public:
	static FGridInventoryCell MakeParent();
	static FGridInventoryCell MakeParent(const FGridItemDefinition& Definition, ALimenItemBase* InItem);
	static FGridInventoryCell MakeParent(const FGridItemDefinition& Definition, const TArray<TWeakObjectPtr<ALimenItemBase>>& InItems);
	static FGridInventoryCell MakeChild(FGridInventoryCell* Parent);
	
	FGridInventoryCell();
	bool IsParent() const;
	bool IsOccupied() const;
	FGridInventoryCell* GetParent() const;
	const TArray<FGridInventoryCell*>& GetChildren() const;
	TWeakObjectPtr<ALimenItemBase> GetItem();
	TArray<TWeakObjectPtr<ALimenItemBase>> GetItems();
	const TArray<TWeakObjectPtr<ALimenItemBase>>& PeekItems() const;
	TSubclassOf<ALimenItemBase> GetItemClass() const;
	void AddChild(FGridInventoryCell* Child);
	const FGridItemDefinition* GetDefinition() const;
	void AddItem(ALimenItemBase* Item);

private:
	FGridInventoryCell* ParentCell;
	TArray<FGridInventoryCell*> ChildCells;
	TArray<TWeakObjectPtr<ALimenItemBase>> Items;
	FGridItemDefinition GridItemDefinition;
};

struct FInventoryCellUpdate
{
	FInventoryCellUpdate() = default;
	FInventoryCellUpdate(const FIntVector2& InCoord, const FGridInventoryCell* InCell);
	
	FIntVector2 Coordinate;
	const FGridInventoryCell* Cell;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FGridInventoryUpdate, const TArray<FInventoryCellUpdate>& /* Updated Cells */)

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenGridInventoryComponent : public UActorComponent, public ILimenInventory
{
	GENERATED_BODY()

public:
	FGridInventoryUpdate OnCellUpdated;
	
	ULimenGridInventoryComponent();
	virtual void BeginPlay() override;
	
	virtual FInventoryUpdate& GetOnInventoryUpdated() override;
	virtual FInventoryUpdate& GetOnInventoryRefreshed() override;
	virtual FInventoryItemUpdate& GetOnItemFailedToAdd() override;
	virtual FInventoryItemUpdate& GetOnItemAdded() override;
	virtual FInventoryItemUpdate& GetOnItemRemoved() override;
	virtual FInventoryItemUpdate& GetOnItemUpdated() override;
	
	bool CanPutItemAt(const ALimenItemBase* Item, const FIntVector2& Coordinates) const;
	bool CanPutStackAt(const TArray<ALimenItemBase*>& Items, const FIntVector2& Coordinates) const;
	bool CanPutStackAt(const TArray<TWeakObjectPtr<ALimenItemBase>>& Items, const FIntVector2& Coordinates) const;
	void AddItem(ALimenItemBase* Item, const FIntVector2& Coordinates);
	bool AddItem(ALimenItemBase* Item);
	void AddItems(TArray<ALimenItemBase*>& Items, const FIntVector2& Coordinates);
	void MoveItem(ALimenItemBase* Item, const FIntVector2& NewCoordinates);
	void MoveItemStack(const FIntVector2& From, const FIntVector2& To);
	ALimenItemBase* GetItem(const FIntVector2& Coordinates);
	TArray<ALimenItemBase*> GetItemStack(const FIntVector2& Coordinates);
	void GetItem(const ALimenItemBase* Item);
	TArray<ALimenItemBase*> PeekItems(const FIntVector2& Coordinates) const;
	TArray<ALimenItemBase*> PeekItems(const TSubclassOf<ALimenItemBase>& Class) const;
	template <typename T> TArray<T*> PeekItems() const;
	bool ContainsItem(const ALimenItemBase* Item) const;
	FIntVector2 GetItemCoordinates(const ALimenItemBase* Item) const;
	FIntVector2 GetSize() const;
	ULimenGridItemDatabase* GetItemDatabase() const;
	
	const FGridInventoryCell* GetCell(const FIntVector2& Coordinates) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	TObjectPtr<ULimenGridItemDatabase> ItemDatabase;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory")
	FIntVector2 Size;
	
private:
	TArray<FGridInventoryCell> Cells;
	
	int32 CoordinateToIndex(const FIntVector2& Coordinate) const;
	FIntVector2 IndexToCoordinate(const int32 Index) const;
	bool IsLocationValid(const FIntVector2& Location) const;
	bool IsRegionValid(const FIntVector2& Origin, const FIntVector2& Extent) const;
	bool IsIndexValid(int32 Index) const;
};

template <typename T>
TArray<T*> ULimenGridInventoryComponent::PeekItems() const
{
	static_assert(TIsDerivedFrom<T, ALimenItemBase>::Value);
	TSet<T*> Out;
	for (int X = 0; X < Size.X; ++X)
		for (int Y = 0; Y < Size.Y; ++Y)
		{
			TArray Items = PeekItems(FIntVector2(X, Y));
			if (Items.IsEmpty()) { continue; }
		
			if (Items[0]->IsA<T>())
			{
				for (auto& Item : Items)
				{
					Out.Add(CastChecked<T>(Item));
				}
			}
		}
	
	return Out.Array();
}
