// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenGameMenuWidget.h"


void ULimenGameMenuWidget::BindPlayerController(APlayerController* InPlayerController)
{	
	Super::BindPlayerController(InPlayerController);
	
	if (!IsValid(InPlayerController) || !IsValid(InPlayerController->GetPawn()))
	{
		return;
	}
	
	BoundPawn = InPlayerController->GetPawn();
	PawnBoundUpdated(BoundPawn.Get());
}

APawn* ULimenGameMenuWidget::GetBoundPawn() const
{
	return BoundPawn.Get();
}
