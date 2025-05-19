// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenGameMenuWidget.h"

#include "GameFramework/Pawn.h"


void ULimenGameMenuWidget::BindPlayerController(APlayerController* InPlayerController)
{
	if (BoundPlayerController.IsValid())
	{
		BoundPlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::PossessedPawnChanged);
	}

	Super::BindPlayerController(InPlayerController);

	if (BoundPlayerController.IsValid())
	{
		BoundPlayerController->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::PossessedPawnChanged);
		BoundPawn = BoundPlayerController->GetPawn();
	}
	else
	{
		BoundPawn.Reset();
	}

	PawnBoundUpdated(BoundPawn.Get());
}

APawn* ULimenGameMenuWidget::GetBoundPawn() const
{
	return BoundPawn.Get();
}

void ULimenGameMenuWidget::PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BoundPawn = NewPawn;
	PawnBoundUpdated(BoundPawn.Get());
}
