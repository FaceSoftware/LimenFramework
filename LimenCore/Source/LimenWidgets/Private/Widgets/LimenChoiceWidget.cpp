// Fill out your copyright notice in the Description page of Project Settings.


#include "Widgets/LimenChoiceWidget.h"


void ULimenChoiceWidget::SetChoicesData(const FGameplayChoiceScreenData& ChoiceData)
{
	ChoiceDataReceived(ChoiceData);
}
void ULimenChoiceWidget::MakeChoice(const FGameplayChoiceData& ChoiceData)
{
	OnChoiceSelected.Broadcast(ChoiceData);
}
