// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenMenuWrapperWidget.h"


ULimenMenuWrapperWidget::ULimenMenuWrapperWidget()
{
	NumChecks = 0;
}

void ULimenMenuWrapperWidget::OnPlayerControllerBound_Implementation(APlayerController* InPlayerController)
{
	Super::OnPlayerControllerBound_Implementation(InPlayerController);
	CheckInitialization_Internal();
}

void ULimenMenuWrapperWidget::PawnBoundUpdated_Implementation(APawn* NewPawn)
{
	Super::PawnBoundUpdated_Implementation(NewPawn);
	CheckInitialization_Internal();
}

void ULimenMenuWrapperWidget::CheckInitialization_Internal()
{
	if (++NumChecks == 2)
	{
		OnWidgetInitialized();
	}
}
