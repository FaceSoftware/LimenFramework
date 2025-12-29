// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LimenGridInventoryWidget.h"

#include "Widgets/LimenGridInventoryEntryWidget.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

SLATE_IMPLEMENT_WIDGET(SLimenGridInventory)

void SLimenGridInventory::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
}

SLimenGridInventory::SLimenGridInventory()
{
}

void SLimenGridInventory::Construct(const FArguments& InArgs)
{	
	SetInventory(InArgs._Inventory);
	CellSize = InArgs._CellSize;
	CellWidget = InArgs._CellWidget;
	
	
	
	CreateInventoryGrid();
	
	ChildSlot
	[
		GridPanel.ToSharedRef()
	];
}

void SLimenGridInventory::SetInventory(ULimenGridInventoryComponent* InInventory)
{
	if (InventoryComponent.IsValid())
	{
		InventoryComponent.Get()->OnCellUpdated.Remove(CellUpdateDelegateHandle);
	}
	
	InventoryComponent = TWeakObjectPtr(InInventory);
	if (InventoryComponent.IsValid())
	{
		InventorySize = InventoryComponent.Get()->GetSize();
		CellUpdateDelegateHandle = InventoryComponent.Get()->OnCellUpdated.AddRaw(this, &ThisClass::InventoryCellUpdate);
	}
	else
	{
		InventorySize = FIntVector2::ZeroValue;
	}
	
	CreateInventoryGrid();
}

FReply SLimenGridInventory::OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	const TOptional<FIntVector2> RootCellCoordinates = GetRootCellCoordinates(MyGeometry, DragDropEvent);
	if (!RootCellCoordinates.IsSet()) { return FReply::Unhandled(); }

	const TSet<FIntVector2> PlacementCells = GetPlacementCells(RootCellCoordinates.GetValue(), DragDropEvent);
	if (PlacementCells.IsEmpty()) { return FReply::Unhandled(); }
	
	ClearHighlightedCells();
	
	const TSharedPtr Operation = DragDropEvent.GetOperationAs<FGridEntryDragDropOperation>();
	if (!Operation.IsValid()) { return FReply::Unhandled(); }
	
	if (const TArray<TWeakObjectPtr<ALimenItemBase>>& GridItems = Operation->GetBoundItems(); !GridItems.IsEmpty())
	{
		const bool bCanAdd = InventoryComponent.Get()->CanPutStackAt(GridItems, RootCellCoordinates.GetValue()) != 0;
		
		for (const FIntVector2& Cell : PlacementCells)
		{
			const int32 Index = Cell.Y * InventorySize.X + Cell.X;
			if (!GridCellWidgets.IsValidIndex(Index)) { return FReply::Unhandled(); }

			const TSharedRef<SLimenGridInventoryEntry>& EntryWidget = GridCellWidgets[Index].ToSharedRef();
			if (bCanAdd)
			{
				EntryWidget->SetValidPlacementHighlight();
			}
			else
			{
				EntryWidget->SetInvalidPlacementHighlight();
			}
			HighlightedCells.Add(Cell);
		}
	}
	
	return FReply::Handled();
}

FReply SLimenGridInventory::OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent)
{
	const TOptional<FIntVector2> RootCellCoordinates = GetRootCellCoordinates(MyGeometry, DragDropEvent);
	if (!RootCellCoordinates.IsSet()) { return FReply::Unhandled(); }
	
	const TSharedPtr Operation = DragDropEvent.GetOperationAs<FGridEntryDragDropOperation>();
	if (!Operation.IsValid()) { return FReply::Unhandled(); }

	if (InventoryComponent->CanPutStackAt(Operation->GetBoundItems(), RootCellCoordinates.GetValue()))
	{
		auto Stack = Operation->GetBoundInventoryComponent()->GetItemStack(Operation->GetCoordinates());
		InventoryComponent->AddItems(Stack, RootCellCoordinates.GetValue());
		
		if (!Stack.IsEmpty()) // return left over items from the stack
		{
			Operation->GetBoundInventoryComponent()->AddItems(Stack, Operation->GetCoordinates());
		}
	}

	ClearHighlightedCells();
	
	return FReply::Handled().EndDragDrop();
}

void SLimenGridInventory::OnDragLeave(const FDragDropEvent& DragDropEvent)
{
	SCompoundWidget::OnDragLeave(DragDropEvent);
	
	ClearHighlightedCells();
}

void SLimenGridInventory::InventoryCellUpdate(const TArray<FInventoryCellUpdate>& Updates)
{
	CreateInventoryGrid();
	ChildSlot
	[
		GridPanel.ToSharedRef()
	];
}

FIntVector2 SLimenGridInventory::CursorPositionToCellCoordinate(const FGeometry& MyGeometry,
	const FVector2D& ScreenCursorPosition) const
{
	// Screen to local space
	const FVector2D LocalPos = MyGeometry.AbsoluteToLocal(ScreenCursorPosition);

	// Local to cell coordinates
	if (CellSize.X <= 0.f || CellSize.Y <= 0.f) { return FIntVector2(INDEX_NONE); }

	// Rounding so it matches the closes position to the mouse cursor
	const int32 DesiredCellX = FMath::RoundHalfFromZero(LocalPos.X / CellSize.X);
	const int32 DesiredCellY = FMath::RoundHalfFromZero(LocalPos.Y / CellSize.Y);

	// Clamp to inventory bounds
	const FIntVector2 Size = InventorySize;
	const int32 TargetCellX = FMath::Clamp(DesiredCellX, 0, Size.X - 1);
	const int32 TargetCellY = FMath::Clamp(DesiredCellY, 0, Size.Y - 1);

	return FIntVector2(TargetCellX, TargetCellY);
}

TOptional<FIntVector2> SLimenGridInventory::GetRootCellCoordinates(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) const
{
	const FVector2D CursorPos = DragDropEvent.GetScreenSpacePosition();
	
	const TSharedPtr Operation = DragDropEvent.GetOperationAs<FGridEntryDragDropOperation>();
	if (!Operation.IsValid()) { return {}; }

	const FIntVector2 CoordinatesOffset = Operation->GetItemGridSize() / 2;
	FIntVector2 RootCellCoordinates = CursorPositionToCellCoordinate(MyGeometry, CursorPos) - CoordinatesOffset;
	RootCellCoordinates.X = FMath::Clamp(RootCellCoordinates.X, 0, InventorySize.X - Operation->GetItemGridSize().X);
	RootCellCoordinates.Y = FMath::Clamp(RootCellCoordinates.Y, 0, InventorySize.Y - Operation->GetItemGridSize().Y);
	
	return RootCellCoordinates;
}

TSet<FIntVector2> SLimenGridInventory::GetPlacementCells(const FIntVector2& RootCell,
	const FDragDropEvent& DragDropEvent)
{
	const TSharedPtr Operation = DragDropEvent.GetOperationAs<FGridEntryDragDropOperation>();
	if (!Operation.IsValid()) { return {}; }
	
	TSet<FIntVector2> PlacementCells;
	for (int Y = RootCell.Y; Y < RootCell.Y + Operation->GetItemGridSize().Y; ++Y)
	for (int X = RootCell.X; X < RootCell.X + Operation->GetItemGridSize().X; ++X)
	{
		PlacementCells.Add(FIntVector2(X, Y));
	}
	
	return PlacementCells;
}

void SLimenGridInventory::CreateInventoryGrid()
{
	GridPanel = SNew(SGridPanel);
	
	TSet<FIntVector2> SkipCoordinates;
	
	GridCellWidgets.Empty(InventorySize.X * InventorySize.Y);
	GridCellWidgets.AddDefaulted(InventorySize.X * InventorySize.Y);

	for (int Y = 0; Y < InventorySize.Y; ++Y)
	{
		for (int X = 0; X < InventorySize.X; ++X)
		{
			const FIntVector2 CurrentCoordinates(X, Y);
			if (SkipCoordinates.Contains(CurrentCoordinates)) { continue; }
			
			TArray<ALimenItemBase*> GridItems = InventoryComponent.Get()->PeekItems(CurrentCoordinates);
			const FGridInventoryCell* Cell = InventoryComponent->GetCell(CurrentCoordinates);

			TSharedRef<SLimenGridInventoryEntry> WidgetEntry = SNew(SLimenGridInventoryEntry)
				.GridCell(Cell)
				.BoundInventoryComponent(InventoryComponent.Get())
				.Coordinates(CurrentCoordinates)
				.GridCellSize(CellSize)
				.CellWidget(CellWidget);
			
			const FIntVector2 ItemSize = Cell->GetDefinition()->Size;
			
			GridPanel->AddSlot(X, Y)
				.HAlign(HAlign_Fill)
				.VAlign(VAlign_Fill)
				.ColumnSpan(ItemSize.X)
				.RowSpan(ItemSize.Y)
			[
				SNew(SBox)
				.WidthOverride(CellSize.X * ItemSize.X)
				.HeightOverride(CellSize.Y * ItemSize.Y)
				[
					WidgetEntry
				]
			];
			
			GridCellWidgets[Y * InventorySize.X + X] = WidgetEntry.ToSharedPtr();
			
			for (int ContainerCoordinateX = CurrentCoordinates.X; ContainerCoordinateX < CurrentCoordinates.X + ItemSize.X; ++ContainerCoordinateX)
			{
				for (int ContainerCoordinateY = CurrentCoordinates.Y; ContainerCoordinateY < CurrentCoordinates.Y + ItemSize.Y; ++ContainerCoordinateY)
				{
					SkipCoordinates.Add(FIntVector2(ContainerCoordinateX, ContainerCoordinateY));
					GridCellWidgets[ContainerCoordinateY * InventorySize.X + ContainerCoordinateX] = WidgetEntry.ToSharedPtr();
				}
			}
		}
	}
}

void SLimenGridInventory::ClearHighlightedCells()
{
	for (const auto& Cell : HighlightedCells)
	{
		const TSharedRef<SLimenGridInventoryEntry>& EntryWidget = GridCellWidgets[Cell.Y * InventorySize.X + Cell.X].ToSharedRef();
		EntryWidget->RemoveHighlight();
	}
	HighlightedCells.Empty();
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION


ULimenGridInventoryWidget::ULimenGridInventoryWidget()
{
}

TSharedRef<SWidget> ULimenGridInventoryWidget::RebuildWidget()
{
	GridInventory = SNew(SLimenGridInventory)
		.CellSize(CellSize)
		.Inventory(GridInventoryComponent.Get())
		.CellWidget(CellWidget);
	
	return GridInventory.ToSharedRef();
}

void ULimenGridInventoryWidget::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	
	GridInventory.Reset();
}

void ULimenGridInventoryWidget::BindInventory(ULimenGridInventoryComponent* InInventory)
{
	GridInventoryComponent = InInventory;
	if (GridInventory.IsValid())
	{
		GridInventory->SetInventory(GridInventoryComponent.Get());
	}
}
