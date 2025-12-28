// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LimenGridInventoryWidget.h"

#include "Slate/LimenGridInventory.h"


ULimenGridInventoryWidget::ULimenGridInventoryWidget()
{
}

TSharedRef<SWidget> ULimenGridInventoryWidget::RebuildWidget()
{
	GridInventory = SNew(SLimenGridInventory)
		.CellBackgroundBrush(CellBackgroundBrush)
		.InvalidPlacementHighlightBrush(InvalidPlacementHighlightBrush)
		.ValidPlacementHighlightBrush(ValidPlacementHighlightBrush)
		.CellSize(CellSize)
		.Inventory(GridInventoryComponent.Get());
	
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
