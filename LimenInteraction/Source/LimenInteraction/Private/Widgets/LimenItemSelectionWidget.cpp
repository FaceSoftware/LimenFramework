// Copyright FaceSoftware. All Rights Reserved.


#include "Widgets/LimenItemSelectionWidget.h"


void ULimenItemSelectionWidget::SetAvailableItemOptions(const TArray<TSubclassOf<ALimenItemBase>>& InOptions)
{
	Options = InOptions;
	AvailableItemsUpdated(InOptions);
}

void ULimenItemSelectionWidget::NotifyItemSelected(ALimenItemBase* Item)
{
	OnItemSelected.Broadcast(Item);
}
