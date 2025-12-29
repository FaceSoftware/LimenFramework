// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/LimenGridInventoryComponent.h"
#include "Widgets/LimenWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "LimenGridInventory.generated.h"


struct FInventoryCellUpdate;
class ULimenGridItemComponent;
class ALimenItemBase;
class ULimenGridInventoryComponent;


UENUM(BlueprintType)
enum class EGridCellHighlightCase : uint8
{
	None,
	InvalidPlacement,
	ValidPlacement,
};


UCLASS()
class LIMENINTERACTION_API ULimenGridCellWidget : public ULimenWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void StackNumberUpdated(int32 NewStackNumber);
	UFUNCTION(BlueprintImplementableEvent)
	void PlacementHighlightCaseChanged(EGridCellHighlightCase NewCase);
	UFUNCTION(BlueprintImplementableEvent)
	void ItemIconChanged(const FSlateBrush& InIcon);
	
protected:
	
private:
};


class FGridEntryDragDropOperation : public FDragDropOperation
{
public:
	DRAG_DROP_OPERATOR_TYPE(FGridEntryDragDropOperation, FDragDropOperation)

	explicit FGridEntryDragDropOperation(const FIntVector2& Coordinates,
										 const FVector2D& GridCellSize,
										 const FGridInventoryCell* InGridInventoryCell,
										 ULimenGridInventoryComponent* BoundInventoryComponent);

	virtual void OnDragged(const FDragDropEvent& DragDropEvent) override;
	virtual TSharedPtr<SWidget> GetDefaultDecorator() const override;
	virtual FVector2D GetDecoratorPosition() const override;
		
	const FSlateBrush& GetItemBrush() const { return GridCell->GetDefinition()->Icon; }
	FIntVector2 GetCoordinates() const { return Coordinates; }
	FIntVector2 GetItemGridSize() const { return GridCell->GetDefinition()->Size; }
	const TArray<TWeakObjectPtr<ALimenItemBase>>& GetBoundItems() const { return GridCell->PeekItems(); }
	ULimenGridInventoryComponent* GetBoundInventoryComponent() const { return BoundInventoryComponent.Get(); }
		
private:
	FVector2D CursorPosition;
	FIntVector2 Coordinates;
	FVector2D GridCellSize;

	const FGridInventoryCell* GridCell;
	
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

		SLATE_ARGUMENT(TArray<TWeakObjectPtr<ULimenGridItemComponent>>, BoundItems)
		SLATE_ARGUMENT(ULimenGridInventoryComponent*, BoundInventoryComponent)

		SLATE_ARGUMENT(const FGridInventoryCell*, GridCell)
		SLATE_ARGUMENT(FIntVector2, Coordinates)
		SLATE_ARGUMENT(FVector2D, GridCellSize)

		SLATE_ARGUMENT(TSubclassOf<ULimenGridCellWidget>, CellWidget)

	SLATE_END_ARGS()
	
	SLimenGridInventoryEntry();
	void Construct(const FArguments& InArgs);
	
	void RemoveHighlight();
	void SetInvalidPlacementHighlight();
	void SetValidPlacementHighlight();

private:
	const FGridInventoryCell* GridCell;
	
	TWeakObjectPtr<ULimenGridInventoryComponent> BoundInventoryComponent;
	
	FIntVector2 Coordinates;
	FVector2D GridCellSize;
	TSubclassOf<ULimenGridCellWidget> CellWidget;
	TStrongObjectPtr<ULimenGridCellWidget> CellWidgetPtr;
	
	virtual FReply OnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnDragDetected(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	
	void CreateWidgetInternal();
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
		SLATE_ARGUMENT(TSubclassOf<ULimenGridCellWidget>, CellWidget)

	SLATE_END_ARGS()
	
	SLimenGridInventory();
	void Construct(const FArguments& InArgs);
	
	void SetInventory(ULimenGridInventoryComponent* InInventory);

private:
	TWeakObjectPtr<ULimenGridInventoryComponent> InventoryComponent;
	
	FIntVector2 InventorySize;
	FVector2D CellSize;
	TSubclassOf<ULimenGridCellWidget> CellWidget;

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
