// Copyright FaceSoftware. All Rights Reserved.


#include "Widgets/LimenChoiceWidget.h"


void ULimenChoiceWidget::SetChoicesData(const FGameplayChoiceScreenData& ChoiceData)
{
	ChoiceDataReceived(ChoiceData);
}
void ULimenChoiceWidget::MakeChoice(const FGameplayChoiceData& ChoiceData)
{
	OnChoiceSelected.Broadcast(ChoiceData);
}
