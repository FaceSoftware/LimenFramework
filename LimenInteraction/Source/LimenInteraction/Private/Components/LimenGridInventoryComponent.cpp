// Copyright FaceSoftware. All rights reserved.


#include "Components/LimenGridInventoryComponent.h"

#include "DataAssets/LimenGridItemDatabase.h"


FGridInventoryCell FGridInventoryCell::MakeParent()
{
	return FGridInventoryCell();
}

FGridInventoryCell FGridInventoryCell::MakeParent(const FGridItemDefinition& Definition, ALimenItemBase* InItem)
{
	FGridInventoryCell Out;
	Out.Items.Push(InItem);
	Out.GridItemDefinition = Definition;
	return Out;
}

FGridInventoryCell FGridInventoryCell::MakeParent(const FGridItemDefinition& Definition, const TArray<TWeakObjectPtr<ALimenItemBase>>& InItems)
{
	FGridInventoryCell Out;
	Out.Items.Append(InItems);

	UClass* Class = nullptr;
	for (const auto& Item : InItems)
	{
		if (!Class) { Class = Item->GetOwner()->GetClass(); }
		check(Class == Item->GetOwner()->GetClass());
	}
	Out.GridItemDefinition = Definition;
	
	return Out;
}

FGridInventoryCell FGridInventoryCell::MakeChild(FGridInventoryCell* Parent)
{
	check(Parent)
	check(Parent->IsParent())
	
	FGridInventoryCell Out;
	Out.ParentCell = Parent;
	return Out;
}

FGridInventoryCell::FGridInventoryCell() : ParentCell(nullptr)
{
}

bool FGridInventoryCell::IsParent() const
{
	return ParentCell == nullptr;
}

bool FGridInventoryCell::IsOccupied() const
{
	if (ParentCell) { return ParentCell->IsOccupied(); }
	return !Items.IsEmpty();
}

FGridInventoryCell* FGridInventoryCell::GetParent() const
{
	return ParentCell;
}

const TArray<FGridInventoryCell*>& FGridInventoryCell::GetChildren() const
{
	if (ParentCell) { return ParentCell->GetChildren(); }
	return ChildCells;
}

TWeakObjectPtr<ALimenItemBase> FGridInventoryCell::GetItem()
{
	if (ParentCell) { return ParentCell->GetItem(); }
	return Items.Pop();
}

TArray<TWeakObjectPtr<ALimenItemBase>> FGridInventoryCell::GetItems()
{
	if (ParentCell) { return ParentCell->GetItems(); }

	auto ItemsTemp = MoveTemp(Items);
	check(Items.IsEmpty())
	return ItemsTemp;
}

const TArray<TWeakObjectPtr<ALimenItemBase>>& FGridInventoryCell::PeekItems() const
{
	if (!ParentCell) { return Items; }
	return ParentCell->PeekItems();
}

TSubclassOf<ALimenItemBase> FGridInventoryCell::GetItemClass() const
{
	if (ParentCell) { return ParentCell->GetItemClass(); }
	return GridItemDefinition.Class;
}

void FGridInventoryCell::AddChild(FGridInventoryCell* Child)
{
	if (ParentCell) { ParentCell->AddChild(Child); return; }
	ChildCells.Push(Child);
}

const FGridItemDefinition* FGridInventoryCell::GetDefinition() const
{
	return &GridItemDefinition;
}

void FGridInventoryCell::AddItem(ALimenItemBase* Item)
{
	if (ParentCell) { ParentCell->AddItem(Item); return; }
	
	check(Item)
	check(Item->GetClass() == GridItemDefinition.Class)
	Items.Push(Item);
}

FInventoryCellUpdate::FInventoryCellUpdate(const FIntVector2& InCoord, const FGridInventoryCell* InCell)
	: Coordinate(InCoord), Cell(InCell)
{
}

ULimenGridInventoryComponent::ULimenGridInventoryComponent()
{
}

void ULimenGridInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	Cells.AddDefaulted(Size.X * Size.Y);
}

FInventoryUpdate& ULimenGridInventoryComponent::GetOnInventoryUpdated()
{
	static FInventoryUpdate Temp;
	return Temp;
}

FInventoryUpdate& ULimenGridInventoryComponent::GetOnInventoryRefreshed()
{
	static FInventoryUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenGridInventoryComponent::GetOnItemFailedToAdd()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenGridInventoryComponent::GetOnItemAdded()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenGridInventoryComponent::GetOnItemRemoved()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

FInventoryItemUpdate& ULimenGridInventoryComponent::GetOnItemUpdated()
{
	static FInventoryItemUpdate Temp;
	return Temp;
}

bool ULimenGridInventoryComponent::CanPutItemAt(const ALimenItemBase* Item, const FIntVector2& Coordinates) const
{
	check(Item)
	
	if (!ItemDatabase) { return false; }
	
	const FGridItemDefinition& ItemDefinition = ItemDatabase->GetItemDefinitionChecked(Item);
	const FIntVector2 ItemSize = ItemDefinition.Size;
	if (!IsRegionValid(Coordinates, ItemSize)) { return false; }	
	
	for (int X = Coordinates.X; X < Coordinates.X + ItemSize.X; ++X)
	{
		for (int Y = Coordinates.Y; Y < Coordinates.Y + ItemSize.Y; ++Y)
		{
			const FIntVector2 CurrentCoordinates(X, Y);
			const int32 CurrentIndex = CoordinateToIndex(CurrentCoordinates);

			check(Cells.IsValidIndex(CurrentIndex));
			
			const FGridInventoryCell& CurrentCell = Cells[CurrentIndex];
			const auto& CurrentCellItems = CurrentCell.PeekItems();
			
			const bool bIsFree = !CurrentCell.IsOccupied();
			const bool bIsSameItem = CurrentCellItems.Contains(Item);
			const bool bCanStack = CurrentCell.GetDefinition()->Class == ItemDefinition.Class && ItemDefinition.bCanStack;

			if (!bIsFree && !bIsSameItem)
			{
				return bCanStack && CurrentCoordinates == Coordinates && CurrentCellItems.Num() < CurrentCell.GetDefinition()->StackLimit;
			}
		}
	}
	
	return true;
}

bool ULimenGridInventoryComponent::CanPutStackAt(const TArray<ALimenItemBase*>& Items, const FIntVector2& Coordinates) const
{
	for (const ALimenItemBase* const& Item : Items)
	{
		check(Item)
		return CanPutItemAt(Item, Coordinates);
	}
	
	return false;
}

bool ULimenGridInventoryComponent::CanPutStackAt(const TArray<TWeakObjectPtr<ALimenItemBase>>& Items, const FIntVector2& Coordinates) const
{
	for (const TWeakObjectPtr<ALimenItemBase>& Item : Items)
	{
		check(Item.IsValid())
		return CanPutItemAt(Item.Get(), Coordinates);
	}
	
	return false;
}

void ULimenGridInventoryComponent::AddItem(ALimenItemBase* Item, const FIntVector2& Coordinates)
{
	if (!ItemDatabase) { return; }
	
	check(CanPutItemAt(Item, Coordinates))

	const FGridItemDefinition ItemDefinition = ItemDatabase->GetItemDefinitionChecked(Item);
	const FIntVector2 ItemSize = ItemDefinition.Size;
	check(IsRegionValid(Coordinates, ItemSize))

	TArray<FInventoryCellUpdate> UpdatedCells;
	const int32 ParentCellIndex = CoordinateToIndex(Coordinates);
	
	if (Cells[ParentCellIndex].IsOccupied())
	{
		Cells[ParentCellIndex].AddItem(Item);
	}
	else
	{
		Cells[ParentCellIndex] = FGridInventoryCell::MakeParent(ItemDefinition, Item);
	}
	UpdatedCells.Push(FInventoryCellUpdate(Coordinates, &Cells[ParentCellIndex]));
	
	
	for (int X = Coordinates.X; X < Coordinates.X + ItemSize.X; ++X)
	{
		for (int Y = Coordinates.Y; Y < Coordinates.Y + ItemSize.Y; ++Y)
		{
			const FIntVector2 CurrentCoordinates(X, Y);
			if (CurrentCoordinates == Coordinates) { continue; }
			
			const int32 CurrentIndex = CoordinateToIndex(CurrentCoordinates);
			check(Cells.IsValidIndex(CurrentIndex));
			
			Cells[CurrentIndex] = FGridInventoryCell::MakeChild(&Cells[ParentCellIndex]);
			Cells[ParentCellIndex].AddChild(&Cells[CurrentIndex]);
			UpdatedCells.Push(FInventoryCellUpdate(CurrentCoordinates, &Cells[CurrentIndex]));
		}
	}
	
	OnCellUpdated.Broadcast(UpdatedCells);
}

bool ULimenGridInventoryComponent::AddItem(ALimenItemBase* Item)
{
	for (int X = 0; X < Size.X; ++X)
	{
		for (int Y = 0; Y < Size.Y; ++Y)
		{
			if (const FIntVector2 CurrentCoordinates(X, Y); CanPutItemAt(Item, CurrentCoordinates))
			{
				AddItem(Item, CurrentCoordinates);
				return true;
			}
		}
	}
	return false;
}

void ULimenGridInventoryComponent::AddItems(TArray<ALimenItemBase*>& Items, const FIntVector2& Coordinates)
{
	while (true)
	{
		if (Items.IsEmpty()) { break; }
		auto* Item = Items.Pop();

		check(!ContainsItem(Item))
		if (!CanPutItemAt(Item, Coordinates))
		{
			Items.Push(Item);
			break;
		}
		AddItem(Item, Coordinates);
	}
}

void ULimenGridInventoryComponent::MoveItem(ALimenItemBase* Item, const FIntVector2& NewCoordinates)
{
	check(ContainsItem(Item))
	check(CanPutItemAt(Item, NewCoordinates))
	
	GetItem(Item);
	AddItem(Item, NewCoordinates);
}

void ULimenGridInventoryComponent::MoveItemStack(const FIntVector2& From, const FIntVector2& To)
{
	for (const TWeakObjectPtr<ALimenItemBase>& Item : Cells[CoordinateToIndex(From)].PeekItems())
	{
		if (!CanPutItemAt(Item.Get(), To)) { break; }
		GetItem(Item.Get());
		AddItem(Item.Get(), To);
	}
}

ALimenItemBase* ULimenGridInventoryComponent::GetItem(const FIntVector2& Coordinates)
{
	if (!ItemDatabase) { return nullptr; }
	
	const int32 CellIndex = CoordinateToIndex(Coordinates);
	FGridInventoryCell* ParentCell = Cells[CellIndex].IsParent() ? &Cells[CellIndex] : Cells[CellIndex].GetParent();
	check(ParentCell)
	
	if (!ParentCell->IsOccupied()) { return nullptr; }
	
	TWeakObjectPtr<ALimenItemBase> OutGridItem = ParentCell->GetItem();
	const FGridItemDefinition& Definition = ItemDatabase->GetItemDefinitionChecked(OutGridItem.Get());
	
	TArray<FInventoryCellUpdate> UpdatedCells;
	if (!ParentCell->IsOccupied())
	{
		for (auto* Child : ParentCell->GetChildren())
		{
			check(Child)
			*Child = FGridInventoryCell::MakeParent();
		}
		*ParentCell = FGridInventoryCell::MakeParent();

		for (int Y = Coordinates.Y; Y < Coordinates.Y + Definition.Size.Y; ++Y)
		for (int X = Coordinates.X; X < Coordinates.X + Definition.Size.X; ++X)
		{
			const FIntVector2 CurrentCoordinates(X, Y);
			UpdatedCells.Push(FInventoryCellUpdate(CurrentCoordinates, &Cells[CoordinateToIndex(CurrentCoordinates)]));
		}
		OnCellUpdated.Broadcast(UpdatedCells);
	}
	else
	{
		const int32 Index = Cells.IndexOfByPredicate([&ParentCell] (const FGridInventoryCell& Test)
		{
			return &Test == ParentCell;
		});
		check(Index != INDEX_NONE)

		UpdatedCells.Push(FInventoryCellUpdate(IndexToCoordinate(Index), ParentCell));
		OnCellUpdated.Broadcast(UpdatedCells);
	}
	
	return OutGridItem.Get();
}

TArray<ALimenItemBase*> ULimenGridInventoryComponent::GetItemStack(const FIntVector2& Coordinates)
{
	TArray<ALimenItemBase*> Out;
	while (auto* Item = GetItem(Coordinates)) { Out.Push(Item); }
	return Out;
}

void ULimenGridInventoryComponent::GetItem(const ALimenItemBase* Item)
{
	check(ContainsItem(Item))
	GetItem(GetItemCoordinates(Item));
}

TArray<ALimenItemBase*> ULimenGridInventoryComponent::PeekItems(const FIntVector2& Coordinates) const
{
	const int32 CellIndex = CoordinateToIndex(Coordinates);
	const FGridInventoryCell* ParentCell = Cells[CellIndex].IsParent() ? &Cells[CellIndex] : Cells[CellIndex].GetParent();
	check(ParentCell)
	
	TArray<ALimenItemBase*> Out;
	for (const auto& Item : ParentCell->PeekItems())
	{
		Out.Push(Item.Get());
	}
	
	return Out;
}

TArray<ALimenItemBase*> ULimenGridInventoryComponent::PeekItems(const TSubclassOf<ALimenItemBase>& Class) const
{
	TSet<ALimenItemBase*> Out;
	for (int X = 0; X < Size.X; ++X)
	for (int Y = 0; Y < Size.Y; ++Y)
	{
		TArray Items = PeekItems(FIntVector2(X, Y));
		if (Items.IsEmpty()) { continue; }
		
		if (Items[0]->IsA(Class))
		{
			Out.Append(Items);
		}
	}
	
	return Out.Array();
}

bool ULimenGridInventoryComponent::ContainsItem(const ALimenItemBase* Item) const
{
	for (const FGridInventoryCell& Cell : Cells)
	{
		if (Cell.PeekItems().Contains(Item))
		{
			return true;
		}
	}
	return false;
}

FIntVector2 ULimenGridInventoryComponent::GetItemCoordinates(const ALimenItemBase* Item) const
{
	int32 Index = 0;
	for (const FGridInventoryCell& Cell : Cells)
	{
		if (Cell.PeekItems().Contains(Item))
		{
			return IndexToCoordinate(Index);
		}
		++Index;
	}

	return FIntVector2(INDEX_NONE);
}

FIntVector2 ULimenGridInventoryComponent::GetSize() const
{
	return Size;
}

ULimenGridItemDatabase* ULimenGridInventoryComponent::GetItemDatabase() const
{
	return ItemDatabase.Get();
}

const FGridInventoryCell* ULimenGridInventoryComponent::GetCell(const FIntVector2& Coordinates) const
{
	return &Cells[CoordinateToIndex(Coordinates)];
}

int32 ULimenGridInventoryComponent::CoordinateToIndex(const FIntVector2& Coordinate) const
{
	check(Coordinate.X >= 0)
	check(Coordinate.Y >= 0)
	check(Coordinate.X < Size.X)
	check(Coordinate.Y < Size.Y)
	return Coordinate.Y * Size.X + Coordinate.X;
}

FIntVector2 ULimenGridInventoryComponent::IndexToCoordinate(const int32 Index) const
{
	check(Index >= 0)
	return FIntVector2(Index % Size.X, Index / Size.X);
}

bool ULimenGridInventoryComponent::IsLocationValid(const FIntVector2& Location) const
{
	if (Location.X < 0 || Location.X >= Size.X || Location.Y < 0 || Location.Y >= Size.Y)
	{
		return false;
	}

	return IsIndexValid(CoordinateToIndex(Location));
}

bool ULimenGridInventoryComponent::IsRegionValid(const FIntVector2& Origin, const FIntVector2& Extent) const
{
	for (int X = 0; X < Extent.X; ++X)
	{
		for (int Y = 0; Y < Extent.Y; ++Y)
		{
			if (!IsLocationValid(Origin + FIntVector2(X, Y)))
			{
				return false;
			}
		}
	}
	
	return true;
}

bool ULimenGridInventoryComponent::IsIndexValid(const int32 Index) const
{
	return Cells.IsValidIndex(Index);
}
