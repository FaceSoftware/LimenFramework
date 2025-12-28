// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"


struct FInventoryCellUpdate;
class ULimenGridItemComponent;
class ALimenItemBase;
class ULimenGridInventoryComponent;


class FGridEntryDragDropOperation : public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FGridEntryDragDropOperation, FDragDropOperation)

	explicit FGridEntryDragDropOperation(const FSlateBrush& InItemBrush,
										 const FIntVector2& Coordinates,
										 const FIntVector2& ItemGridSize,
										 ULimenGridItemComponent* BoundItem,
										 const FVector2D& GridCellSize,
										 ULimenGridInventoryComponent* BoundInventoryComponent);
	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override;
	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;
	virtual FVector2D GetDecoratorPosition() const override;
		
	FSlateBrush GetItemBrush() const { return ItemBrush; }
	FIntVector2 GetCoordinates() const { return Coordinates; }
	FIntVector2 GetItemGridSize() const { return ItemGridSize; }
	ULimenGridItemComponent* GetBoundItem() const { return BoundItem.Get(); }
	ULimenGridInventoryComponent* GetBoundInventoryComponent() const { return BoundInventoryComponent.Get(); }
		
private:
	FVector2D CursorPosition;
	FSlateBrush ItemBrush;
	FIntVector2 Coordinates;
	FIntVector2 ItemGridSize;
	FVector2D GridCellSize;
	TWeakObjectPtr<ULimenGridItemComponent> BoundItem;
	TWeakObjectPtr<ULimenGridInventoryComponent> BoundInventoryComponent;
};


/**
 * 
 */
class LIMENINTERACTION_API SLimenGridInventoryEntry : public SCompoundWidget
{	
	SLATE_DECLARE_WIDGET(SLimenGridInventoryEntry, SCompoundWidget)

public:	
	SLATE_BEGIN_ARGS(SLimenGridInventoryEntry)
		{
		}

		SLATE_ARGUMENT(ULimenGridItemComponent*, BoundItem)
		SLATE_ARGUMENT(ULimenGridInventoryComponent*, BoundInventoryComponent)

		SLATE_ARGUMENT(FSlateBrush, DefaultBackgroundBrush)
		SLATE_ARGUMENT(FSlateBrush, InvalidPlacementHighlightBrush)
		SLATE_ARGUMENT(FSlateBrush, ValidPlacementHighlightBrush)

		SLATE_ARGUMENT(FSlateBrush, ItemBrush)
		SLATE_ARGUMENT(FIntVector2, Coordinates)
		SLATE_ARGUMENT(FVector2D, GridCellSize)

	SLATE_END_ARGS()
	
	SLimenGridInventoryEntry();
	void Construct(const FArguments& InArgs);
	
	void RemoveHighlight();
	void SetInvalidPlacementHighlight();
	void SetValidPlacementHighlight();

private:
	FSlateBrush DefaultBackgroundBrush;

	FSlateBrush InvalidPlacementHighlightBrush;
	FSlateBrush ValidPlacementHighlightBrush;

	FSlateBrush ItemBrush;
	
	TWeakObjectPtr<ULimenGridItemComponent> BoundItem;
	TWeakObjectPtr<ULimenGridInventoryComponent> BoundInventoryComponent;
	FIntVector2 Coordinates;
	FVector2D GridCellSize;
	
	TSharedPtr<SImage> HighlightImage;
	
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	void CreateWidget();
};


class LIMENINTERACTION_API SLimenGridInventory : public SCompoundWidget
{	
	SLATE_DECLARE_WIDGET(SLimenGridInventory, SCompoundWidget)

public:	
	
	SLATE_BEGIN_ARGS(SLimenGridInventory)
		{
		}

		SLATE_ARGUMENT(ULimenGridInventoryComponent*, Inventory)
		SLATE_ARGUMENT(FVector2D, CellSize)
		SLATE_ARGUMENT(FSlateBrush, CellBackgroundBrush)
		SLATE_ARGUMENT(FSlateBrush, InvalidPlacementHighlightBrush)
		SLATE_ARGUMENT(FSlateBrush, ValidPlacementHighlightBrush)

	SLATE_END_ARGS()
	
	SLimenGridInventory();
	void Construct(const FArguments& InArgs);
	
	void SetInventory(ULimenGridInventoryComponent* InInventory);

private:
	TWeakObjectPtr<ULimenGridInventoryComponent> InventoryComponent;
	
	FIntVector2 InventorySize;
	FVector2D CellSize;

	FSlateBrush CellBackgroundBrush;
	FSlateBrush InvalidPlacementHighlightBrush;
	FSlateBrush ValidPlacementHighlightBrush;
	
	TSharedPtr<SGridPanel> GridPanel;
	
	FDelegateHandle CellUpdateDelegateHandle;
	
	TSet<FIntVector2> HighlightedCells;
	
	TArray<TSharedPtr<SLimenGridInventoryEntry>> GridCellWidgets;
	
	virtual FReply OnDragOver(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual FReply OnDrop(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) override;
	virtual void OnDragLeave(const FDragDropEvent& DragDropEvent) override;
	
	virtual void InventoryCellUpdate(const TArray<FInventoryCellUpdate>& Updates);
	
	FIntVector2 CursorPositionToCellCoordinate(const FGeometry& MyGeometry, const FVector2D& ScreenCursorPosition) const;
	TOptional<FIntVector2> GetRootCellCoordinates(const FGeometry& MyGeometry, const FDragDropEvent& DragDropEvent) const;
	static TSet<FIntVector2> GetPlacementCells(const FIntVector2& RootCell, const FDragDropEvent& DragDropEvent);
	
	void CreateInventoryGrid();
	void ClearHighlightedCells();
};
