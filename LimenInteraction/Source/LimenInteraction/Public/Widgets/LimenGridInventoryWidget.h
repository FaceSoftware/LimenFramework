// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "LimenGridInventoryWidget.generated.h"


class ULimenGridCellWidget;
class ULimenGridInventoryComponent;
class SLimenGridInventory;




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

/**
 * 
 */
UCLASS()
class LIMENINTERACTION_API ULimenGridInventoryWidget : public UWidget
{
	GENERATED_BODY()
	
public:
	ULimenGridInventoryWidget();
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;
	
	UFUNCTION(BlueprintCallable)
	void BindInventory(ULimenGridInventoryComponent* InInventory);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Inventory")
	TSubclassOf<ULimenGridCellWidget> CellWidget;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Inventory")
	FVector2D CellSize;

private:
	TSharedPtr<SLimenGridInventory> GridInventory;
	TWeakObjectPtr<ULimenGridInventoryComponent> GridInventoryComponent;
};
