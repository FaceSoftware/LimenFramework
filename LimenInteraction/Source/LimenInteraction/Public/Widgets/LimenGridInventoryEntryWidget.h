// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/LimenGridInventoryComponent.h"
#include "Widgets/LimenWidget.h"
#include "Widgets/SCompoundWidget.h"
#include "LimenGridInventoryEntryWidget.generated.h"


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

USTRUCT(BlueprintType)
struct FInventoryGridCellData
{
	GENERATED_BODY()
	
	FInventoryGridCellData();
	
	UPROPERTY(BlueprintReadOnly)
	bool bHasItem;
	UPROPERTY(BlueprintReadOnly)
	int32 StackNumber;
	UPROPERTY(BlueprintReadOnly)
	FSlateBrush Icon;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ALimenItemBase> Item;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenGridInventoryComponent> Inventory;
};


UCLASS()
class LIMENINTERACTION_API ULimenGridCellWidget : public ULimenWidget
{
	GENERATED_BODY()
	
public:
	virtual void SetPlacementHighlightCase(EGridCellHighlightCase NewCase);
	virtual void SetCellData(const FInventoryGridCellData& InData);
	
protected:
	const FInventoryGridCellData& GetCellData() const;
	
	UFUNCTION(BlueprintNativeEvent)
	void PlacementHighlightCaseChanged(EGridCellHighlightCase NewCase);
	UFUNCTION(BlueprintNativeEvent)
	void CellDataSet(const FInventoryGridCellData& InData);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	FInventoryGridCellData CellData;
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess = true))
	EGridCellHighlightCase CurrentHighlightCase;
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