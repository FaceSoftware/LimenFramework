// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LimenGridInventoryComponent.h"


ULimenGridItemComponent::ULimenGridItemComponent()
{
}

FIntVector2 ULimenGridItemComponent::GetSize() const
{
	return Size;
}

bool ULimenGridItemComponent::CanStack() const
{
	return bCanStack;
}

FSlateBrush ULimenGridItemComponent::GetIcon()
{
	return ItemBrush;
}

FGridInventoryCell FGridInventoryCell::MakeParent(ULimenGridItemComponent* InItem)
{
	FGridInventoryCell Out;
	Out.Item = InItem;
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
	if (IsParent())
	{
		return Item != nullptr;
	}
	
	return ParentCell->IsOccupied();
}

FGridInventoryCell* FGridInventoryCell::GetParent() const
{
	return ParentCell;
}

const TArray<FGridInventoryCell*>& FGridInventoryCell::GetChildren() const
{
	return ChildCells;
}

ULimenGridItemComponent* FGridInventoryCell::GetItem()
{
	ULimenGridItemComponent* Temp = Item.Get();
	Item.Reset();
	return Temp;
}

ULimenGridItemComponent* FGridInventoryCell::PeekItem() const
{
	if (!ParentCell) { return Item.Get(); }
	return ParentCell->PeekItem();
}

void FGridInventoryCell::AddChild(FGridInventoryCell* Child)
{
	ChildCells.Push(Child);
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

bool ULimenGridInventoryComponent::CanPutItemAt(const ULimenGridItemComponent* GridItem, const FIntVector2& Coordinates) const
{
	check(GridItem)
	
	const FIntVector2 ItemSize = GridItem->GetSize();
	if (!IsRegionValid(Coordinates, ItemSize)) { return false; }

	for (int X = Coordinates.X; X < Coordinates.X + ItemSize.X; ++X)
	{
		for (int Y = Coordinates.Y; Y < Coordinates.Y + ItemSize.Y; ++Y)
		{
			const FIntVector2 CurrentCoordinates(X, Y);
			const int32 CurrentIndex = CoordinateToIndex(CurrentCoordinates);
			check(Cells.IsValidIndex(CurrentIndex));
			
			if (Cells[CurrentIndex].IsOccupied() && Cells[CurrentIndex].PeekItem() != GridItem)
			{
				return false;
			}
		}
	}
	
	return true;
}

void ULimenGridInventoryComponent::AddItem(ULimenGridItemComponent* GridItem, const FIntVector2& Coordinates)
{
	check(CanPutItemAt(GridItem, Coordinates))
	
	const FIntVector2 ItemSize = GridItem->GetSize();
	check(IsRegionValid(Coordinates, ItemSize))

	const int32 ParentCellIndex = CoordinateToIndex(Coordinates);
	
	TArray<FInventoryCellUpdate> UpdatedCells;
	
	for (int X = Coordinates.X; X < Coordinates.X + ItemSize.X; ++X)
	{
		for (int Y = Coordinates.Y; Y < Coordinates.Y + ItemSize.Y; ++Y)
		{
			const FIntVector2 CurrentCoordinates(X, Y);
			const int32 CurrentIndex = CoordinateToIndex(CurrentCoordinates);
			check(Cells.IsValidIndex(CurrentIndex));
			
			UpdatedCells.Push(FInventoryCellUpdate(CurrentCoordinates, &Cells[CurrentIndex]));
			
			if (CurrentIndex == ParentCellIndex) // Is the parent cell
			{
				Cells[ParentCellIndex] = FGridInventoryCell::MakeParent(GridItem);
			}
			else // Child cells
			{
				Cells[CurrentIndex] = FGridInventoryCell::MakeChild(&Cells[ParentCellIndex]);
				Cells[ParentCellIndex].AddChild(&Cells[CurrentIndex]);
			}
		}
	}
	
	OnCellUpdated.Broadcast(UpdatedCells);
}

bool ULimenGridInventoryComponent::AddItem(ULimenGridItemComponent* GridItem)
{
	for (int X = 0; X < Size.X; ++X)
	{
		for (int Y = 0; Y < Size.Y; ++Y)
		{
			if (const FIntVector2 CurrentCoordinates(X, Y); CanPutItemAt(GridItem, CurrentCoordinates))
			{
				AddItem(GridItem, CurrentCoordinates);
				return true;
			}
		}
	}
	return false;
}

void ULimenGridInventoryComponent::MoveItem(ULimenGridItemComponent* GridItem, const FIntVector2& NewCoordinates)
{
	check(ContainsItem(GridItem))
	check(CanPutItemAt(GridItem, NewCoordinates))
	
	GetItem(GridItem);
	AddItem(GridItem, NewCoordinates);
}

ULimenGridItemComponent* ULimenGridInventoryComponent::GetItem(const FIntVector2& Coordinates)
{
	const int32 CellIndex = CoordinateToIndex(Coordinates);
	FGridInventoryCell* ParentCell = Cells[CellIndex].IsParent() ? &Cells[CellIndex] : Cells[CellIndex].GetParent();
	check(ParentCell)
	
	ULimenGridItemComponent* OutGridItem = ParentCell->GetItem();
	
	for (auto* Child : ParentCell->GetChildren())
	{
		check(Child)
		*Child = FGridInventoryCell::MakeParent();
	}
	*ParentCell = FGridInventoryCell::MakeParent();
	
	TArray<FInventoryCellUpdate> UpdatedCells;
	for (int Y = Coordinates.Y; Y < Coordinates.Y + OutGridItem->GetSize().Y; ++Y)
	for (int X = Coordinates.X; X < Coordinates.X + OutGridItem->GetSize().X; ++X)
	{
		const FIntVector2 CurrentCoordinates(X, Y);
		UpdatedCells.Push(FInventoryCellUpdate(CurrentCoordinates, &Cells[CoordinateToIndex(CurrentCoordinates)]));
	}
	OnCellUpdated.Broadcast(UpdatedCells);
	
	return OutGridItem;
}

void ULimenGridInventoryComponent::GetItem(const ULimenGridItemComponent* Item)
{
	check(ContainsItem(Item))
	GetItem(GetItemCoordinates(Item));
}

ULimenGridItemComponent* ULimenGridInventoryComponent::PeekItem(const FIntVector2& Coordinates) const
{
	const int32 CellIndex = CoordinateToIndex(Coordinates);
	const FGridInventoryCell* ParentCell = Cells[CellIndex].IsParent() ? &Cells[CellIndex] : Cells[CellIndex].GetParent();
	check(ParentCell)
	
	return ParentCell->PeekItem();
}

bool ULimenGridInventoryComponent::ContainsItem(const ULimenGridItemComponent* GridItem) const
{
	for (const FGridInventoryCell& Cell : Cells)
	{
		if (Cell.PeekItem() == GridItem)
		{
			return true;
		}
	}
	return false;
}

FIntVector2 ULimenGridInventoryComponent::GetItemCoordinates(const ULimenGridItemComponent* GridItem) const
{
	int32 Index = 0;
	for (const FGridInventoryCell& Cell : Cells)
	{
		if (Cell.PeekItem() == GridItem)
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
