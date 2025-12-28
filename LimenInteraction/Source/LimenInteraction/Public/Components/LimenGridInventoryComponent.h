// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/LimenInventory.h"
#include "LimenGridInventoryComponent.generated.h"


class ALimenItemBase;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ULimenGridItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenGridItemComponent();
	
	FIntVector2 GetSize() const;
	bool CanStack() const;
	FSlateBrush GetIcon();
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Item")
	FIntVector2 Size;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Item")
	bool bCanStack;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Grid Item")
	FSlateBrush ItemBrush;
};


class FGridInventoryCell
{
public:
	static FGridInventoryCell MakeParent(ULimenGridItemComponent* InItem = nullptr);
	static FGridInventoryCell MakeChild(FGridInventoryCell* Parent);
	
	FGridInventoryCell();
	bool IsParent() const;
	bool IsOccupied() const;
	FGridInventoryCell* GetParent() const;
	const TArray<FGridInventoryCell*>& GetChildren() const;
	ULimenGridItemComponent* GetItem();
	ULimenGridItemComponent* PeekItem() const;
	void AddChild(FGridInventoryCell* Child);

private:
	FGridInventoryCell* ParentCell;
	TArray<FGridInventoryCell*> ChildCells;
	TWeakObjectPtr<ULimenGridItemComponent> Item;
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
	
	bool CanPutItemAt(const ULimenGridItemComponent* GridItem, const FIntVector2& Coordinates) const;
	void AddItem(ULimenGridItemComponent* GridItem, const FIntVector2& Coordinates);
	bool AddItem(ULimenGridItemComponent* GridItem);
	void MoveItem(ULimenGridItemComponent* GridItem, const FIntVector2& NewCoordinates);
	ULimenGridItemComponent* GetItem(const FIntVector2& Coordinates);
	void GetItem(const ULimenGridItemComponent* Item);
	ULimenGridItemComponent* PeekItem(const FIntVector2& Coordinates) const;
	bool ContainsItem(const ULimenGridItemComponent* GridItem) const;
	FIntVector2 GetItemCoordinates(const ULimenGridItemComponent* GridItem) const;
	
	FIntVector2 GetSize() const;

protected:
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
