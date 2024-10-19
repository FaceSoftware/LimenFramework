// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenInventoryWidget.h"


void ULimenInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void ULimenInventoryWidget::BindInventory(ULimenInventoryComponent* NewBoundInventory)
{
	BoundInventory = NewBoundInventory;
	InventoryBound(BoundInventory.Get());
}

ULimenInventoryComponent* ULimenInventoryWidget::GetBoundInventory() const
{
	return BoundInventory.Get();
}
