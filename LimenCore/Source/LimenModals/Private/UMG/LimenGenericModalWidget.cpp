// Copyright Face Software. All Rights Reserved.


#include "UMG/LimenGenericModalWidget.h"


ULimenGenericModalWidget::ULimenGenericModalWidget(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	SetWidgetLevel(20);
	SetDefaultVisibleState(ESlateVisibility::Visible);
	SetDefaultHiddenState(ESlateVisibility::Collapsed);
}

void ULimenGenericModalWidget::SetParams(const FModalParams& InParams)
{
	ModalParams = InParams;
}

void ULimenGenericModalWidget::ModalDismissed(const bool bAccepted)
{
	DestroyWidget();
	OnModalResponseReceived.Broadcast(bAccepted);
}
