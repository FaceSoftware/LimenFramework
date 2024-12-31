// Copyright Face Software. All Rights Reserved.


#include "UMG/LimenGenericModalWidget.h"


FModalParams::FModalParams(const FString& InTitle, const FString& InContent)
	: Title(FText::FromString(InTitle)), Content(FText::FromString(InContent))
{
}

FModalParams::FModalParams(const FText& InTitle, const FText& InContent)
	: Title(InTitle), Content(InContent)
{
}

ULimenGenericModalWidget::ULimenGenericModalWidget(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	SetWidgetLevel(20);
	SetDefaultVisibleState(ESlateVisibility::Visible);
	SetDefaultHiddenState(ESlateVisibility::Collapsed);
}

void ULimenGenericModalWidget::SetParams(const FModalParams& InParams)
{
	ModalParams = InParams;
	ModalParamsUpdated(ModalParams);
}

void ULimenGenericModalWidget::ModalDismissed(const bool bAccepted)
{
	DestroyWidget();
	OnModalResponseReceived.Broadcast(this, bAccepted);
}
