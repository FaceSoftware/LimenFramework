// Fill out your copyright notice in the Description page of Project Settings.


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
