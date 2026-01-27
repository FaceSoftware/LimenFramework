// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LimenGridInventoryEntryWidget.h"

#include "Components/LimenGridInventoryComponent.h"


BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

FInventoryGridCellData::FInventoryGridCellData()
{
	bHasItem = false;
	StackNumber = INDEX_NONE;
	Icon.TintColor = FSlateColor(FLinearColor::Transparent);
	Inventory = nullptr;
}

void ULimenGridCellWidget::SetPlacementHighlightCase(EGridCellHighlightCase NewCase)
{
	PlacementHighlightCaseChanged(NewCase);
}

void ULimenGridCellWidget::SetCellData(const FInventoryGridCellData& InData)
{
	CellData = InData;
	CellDataSet(InData);
}

const FInventoryGridCellData& ULimenGridCellWidget::GetCellData() const
{
	return CellData;
}

void ULimenGridCellWidget::CellDataSet_Implementation(const FInventoryGridCellData& InData)
{
}

void ULimenGridCellWidget::PlacementHighlightCaseChanged_Implementation(EGridCellHighlightCase NewCase)
{
}

FGridEntryDragDropOperation::FGridEntryDragDropOperation(const FIntVector2& Coordinates,
                                                         const FVector2D& GridCellSize,
                                                         const FGridInventoryCell* InGridInventoryCell,
                                                         ULimenGridInventoryComponent* BoundInventoryComponent)
															 : 
														 Coordinates(Coordinates),
														 GridCellSize(GridCellSize),
														 GridCell(InGridInventoryCell),
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
		.WidthOverride(GridCellSize.X * GetItemGridSize().X)
		.HeightOverride(GridCellSize.Y * GetItemGridSize().Y)
		[
			SNew(SImage).Image(&GetItemBrush())
		];
}

FVector2D FGridEntryDragDropOperation::GetDecoratorPosition() const
{
	return CursorPosition - GridCellSize * FVector2D(GetItemGridSize().X, GetItemGridSize().Y) * 0.5f;
}

SLATE_IMPLEMENT_WIDGET(SLimenGridInventoryEntry)

void SLimenGridInventoryEntry::PrivateRegisterAttributes(FSlateAttributeInitializer& AttributeInitializer)
{
}

SLimenGridInventoryEntry::SLimenGridInventoryEntry() : GridCell(nullptr)
{
}

void SLimenGridInventoryEntry::Construct(const FArguments& InArgs)
{	
	GridCell = InArgs._GridCell;

	BoundInventoryComponent = InArgs._BoundInventoryComponent;

	Coordinates = InArgs._Coordinates;
	GridCellSize = InArgs._GridCellSize;
	CellWidget = InArgs._CellWidget;
	
	CreateWidgetInternal();
}

void SLimenGridInventoryEntry::RemoveHighlight()
{
	if (CellWidgetPtr.IsValid())
	{
		CellWidgetPtr->SetPlacementHighlightCase(EGridCellHighlightCase::None);
	}
}

void SLimenGridInventoryEntry::SetInvalidPlacementHighlight()
{
	if (CellWidgetPtr.IsValid())
	{
		CellWidgetPtr->SetPlacementHighlightCase(EGridCellHighlightCase::InvalidPlacement);
	}
}

void SLimenGridInventoryEntry::SetValidPlacementHighlight()
{
	if (CellWidgetPtr.IsValid())
	{
		CellWidgetPtr->SetPlacementHighlightCase(EGridCellHighlightCase::ValidPlacement);
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
	if (GridCell->PeekItems().IsEmpty()) { return FReply::Unhandled(); }
	const TSharedRef Op = MakeShared<FGridEntryDragDropOperation>(Coordinates, GridCellSize, GridCell, BoundInventoryComponent.Get());
	return FReply::Handled().BeginDragDrop(Op);
}

void SLimenGridInventoryEntry::CreateWidgetInternal()
{	
	if (!CellWidget) { return; }
	
	UWorld* World = BoundInventoryComponent->GetWorld();
	CellWidgetPtr = TStrongObjectPtr(CreateWidget<ULimenGridCellWidget>(World, CellWidget));
	if (!CellWidgetPtr.IsValid()) { checkNoEntry() return; }
	
	FInventoryGridCellData Data;
	Data.bHasItem = GridCell->IsOccupied();
	Data.StackNumber = GridCell->PeekItems().Num();
	Data.Icon = GridCell->GetDefinition()->Icon;
	Data.Inventory = BoundInventoryComponent.Get();
	Data.Item = GridCell->IsOccupied() ? GridCell->PeekItems()[0].Get() : nullptr;
	CellWidgetPtr->SetCellData(Data);
	
	ChildSlot
	[
		CellWidgetPtr->TakeWidget()
	];
}


END_SLATE_FUNCTION_BUILD_OPTIMIZATION
