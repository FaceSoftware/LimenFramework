// Copyright Face Software. All Rights Reserved.


#include "UMG/LimenLoadingScreenWidget.h"

#include "Subsystems/LimenLevelTransitionSubsystem.h"


ULimenLoadingScreenWidget::ULimenLoadingScreenWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetWidgetLevel(40);
}

void ULimenLoadingScreenWidget::ShowWidgetMethod()
{
	UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
	check(ViewportClient != nullptr);
	ViewportClient->AddViewportWidgetContent(TakeWidget(), GetWidgetLevel());
}
