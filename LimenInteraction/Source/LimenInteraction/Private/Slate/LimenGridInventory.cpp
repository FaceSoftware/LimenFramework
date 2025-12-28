// Fill out your copyright notice in the Description page of Project Settings.


#include "Slate/LimenGridInventory.h"

#include "Components/LimenGridInventoryComponent.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FGridEntryDragDropOperation::FGridEntryDragDropOperation(const FSlateBrush& InItemBrush, 
														 const FIntVector2& Coordinates,
														 const FIntVector2& ItemGridSize,
														 ULimenGridItemComponent* BoundItem,
														 const FVector2D& GridCellSize,
														 ULimenGridInventoryComponent* BoundInventoryComponent)
														 :
														 ItemBrush(InItemBrush),
														 Coordinates(Coordinates),
														 ItemGridSize(ItemGridSize),
														 GridCellSize(GridCellSize),
														 BoundItem(BoundItem),
														 BoundInventoryComponent(BoundInventoryComponent)
{
	bCreateNewWindow = false;
}

void FGridEntryDragDropOperation::OnDragged(const FDragDropEvent& DragDropEvent)
{
	FDragDropOperation::OnDragged(DragDropEvent);
	CursorPosition = DragDropEvent.GetScreenSpacePosition();
}

TSharedPtr<SWidget> FGridEntryDragDropOperation::GetDefaultDecorator() const
{
	return SNew(SBox)
		.WidthOverride(GridCellSize.X * ItemGridSize.X)
		.HeightOverride(GridCellSize.Y * ItemGridSize.Y)
		[
			SNew(SImage).Image(&ItemBrush)
		];
}

FVector2D FGridEntryDragDropOperation::GetDecoratorPosition() const
{
	return CursorPosition - GridCellSize * FVector2D(ItemGridSize.X, ItemGridSize.Y) * 0.5f;
}

SLATE_IMPLEMENT_WIDGET(SLimenGridInventoryEntry)

void SLimenGridInventoryEntry::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
}

SLimenGridInventoryEntry::SLimenGridInventoryEntry()
{
}

void SLimenGridInventoryEntry::Construct(const FArguments& InArgs)
{
	DefaultBackgroundBrush = InArgs._DefaultBackgroundBrush;
	InvalidPlacementHighlightBrush = InArgs._InvalidPlacementHighlightBrush;
	ValidPlacementHighlightBrush = InArgs._ValidPlacementHighlightBrush;

	ItemBrush = InArgs._ItemBrush;
	
	BoundItem = InArgs._BoundItem;
	BoundInventoryComponent = InArgs._BoundInventoryComponent;
	Coordinates = InArgs._Coordinates;
	GridCellSize = InArgs._GridCellSize;
	
	CreateWidget();
}

void SLimenGridInventoryEntry::RemoveHighlight()
{
	if (HighlightImage.IsValid())
	{
		HighlightImage->SetVisibility(EVisibility::Hidden);
	}
}

void SLimenGridInventoryEntry::SetInvalidPlacementHighlight()
{
	if (HighlightImage.IsValid())
	{
		HighlightImage->SetImage(&InvalidPlacementHighlightBrush);
		HighlightImage->SetVisibility(EVisibility::Visible);
	}
}

void SLimenGridInventoryEntry::SetValidPlacementHighlight()
{
	if (HighlightImage.IsValid())
	{
		HighlightImage->SetImage(&ValidPlacementHighlightBrush);
		HighlightImage->SetVisibility(EVisibility::Visible);
	}
}

FReply SLimenGridInventoryEntry::OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(SharedThis(this), EKeys::LeftMouseButton);
	}
	
	return FReply::Unhandled();
}

FReply SLimenGridInventoryEntry::OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	if (!BoundItem.IsValid()) { return FReply::Unhandled(); }
	const TSharedRef Op = MakeShared<FGridEntryDragDropOperation>(ItemBrush,
																  Coordinates,
																  BoundItem->GetSize(),
																  BoundItem.Get(),
																  GridCellSize,
																  BoundInventoryComponent.Get());
	return FReply::Handled().BeginDragDrop(Op);
}

void SLimenGridInventoryEntry::CreateWidget()
{
	TSharedRef BackgroundImage = SNew(SImage)
			.Image(&DefaultBackgroundBrush);
	
	HighlightImage = SNew(SImage)
		.Visibility(EVisibility::Hidden);

	const TSharedRef<SOverlay> Root = SNew(SOverlay)
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			BackgroundImage
		]
		+SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			HighlightImage.ToSharedRef()
		];
	
	if (BoundItem.IsValid())
	{
		Root->AddSlot()
			.HAlign(HAlign_Fill)
			.VAlign(VAlign_Fill)
			[
				SNew(SImage)
				.Image(&ItemBrush)
			];
	}
	
	ChildSlot
	[
		Root
	];
}

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
	CellBackgroundBrush = InArgs._CellBackgroundBrush;
	InvalidPlacementHighlightBrush = InArgs._InvalidPlacementHighlightBrush;
	ValidPlacementHighlightBrush = InArgs._ValidPlacementHighlightBrush;
	
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
	
	if (const ULimenGridItemComponent* GridItem = Operation->GetBoundItem())
	{
		const bool bCanAdd = InventoryComponent.Get()->CanPutItemAt(GridItem, RootCellCoordinates.GetValue());
		
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

	if (ULimenGridItemComponent* GridItem = Operation->GetBoundInventoryComponent()->GetItem(Operation->GetCoordinates()))
	{
		if (InventoryComponent.Get()->CanPutItemAt(GridItem, RootCellCoordinates.GetValue()))
		{
			InventoryComponent->AddItem(GridItem, RootCellCoordinates.GetValue());
		}
		else
		{
			InventoryComponent.Get()->AddItem(GridItem, Operation->GetCoordinates());
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
	// Screen → local space
	const FVector2D LocalPos = MyGeometry.AbsoluteToLocal(ScreenCursorPosition);

	// Local → cell coordinates
	const FVector2D CellSizePx = CellSize;
	if (CellSizePx.X <= 0.f || CellSizePx.Y <= 0.f)
	{
		return FIntVector2(-1, -1);
	}

	int32 CellX = FMath::FloorToInt(LocalPos.X / CellSizePx.X);
	int32 CellY = FMath::FloorToInt(LocalPos.Y / CellSizePx.Y);

	// Clamp to inventory bounds
	const FIntVector2 Size = InventorySize;
	CellX = FMath::Clamp(CellX, 0, Size.X - 1);
	CellY = FMath::Clamp(CellY, 0, Size.Y - 1);

	return FIntVector2(CellX, CellY);
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
			
			ULimenGridItemComponent* GridItem = InventoryComponent.Get()->PeekItem(CurrentCoordinates);

			TSharedRef<SLimenGridInventoryEntry> WidgetEntry = SNew(SLimenGridInventoryEntry)
				.DefaultBackgroundBrush(CellBackgroundBrush)
				.InvalidPlacementHighlightBrush(InvalidPlacementHighlightBrush)
				.ValidPlacementHighlightBrush(ValidPlacementHighlightBrush)
				.ItemBrush(GridItem ? GridItem->GetIcon() : FSlateBrush())
				.BoundItem(GridItem)
				.BoundInventoryComponent(InventoryComponent.Get())
				.Coordinates(CurrentCoordinates)
				.GridCellSize(CellSize);
			
			const FIntVector2 ItemSize = GridItem ? GridItem->GetSize() : FIntVector2(1, 1);
			
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
