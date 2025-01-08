// Copyright Face Software. All Rights Reserved.


#include "UMG/LimenLoadingScreenWidget.h"

#include "Engine/GameViewportClient.h"


ULimenLoadingScreenWidget::ULimenLoadingScreenWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetWidgetLevel(40);
	bIsShowing = false;
}

bool ULimenLoadingScreenWidget::IsShowing() const
{
	return GetVisibility() == GetDefaultVisibleState() && bIsShowing;
}

bool ULimenLoadingScreenWidget::IsHiding() const
{
	return GetVisibility() == GetDefaultHiddenState() || !bIsShowing;
}

void ULimenLoadingScreenWidget::ShowWidgetMethod()
{
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	ViewportClient->AddViewportWidgetContent(TakeWidget(), GetWidgetLevel());
	SetVisibility(GetDefaultVisibleState());

	bIsShowing = true;
}

void ULimenLoadingScreenWidget::HideWidgetMethod()
{
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	ViewportClient->RemoveViewportWidgetContent(TakeWidget());
	SetVisibility(GetDefaultHiddenState());

	bIsShowing = false;
}
