// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenBaseHudWidget.h"

#include "Blueprint/WidgetTree.h"
#include "GameFramework/Pawn.h"


ULimenBaseHudWidget::ULimenBaseHudWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetWidgetLevel(1);
}

void ULimenBaseHudWidget::BindPlayerController(APlayerController* InPlayerController)
{
	if (BoundPlayerController.IsValid())
	{
		BoundPlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::PossessedPawnChanged);
	}
	
	BoundPlayerController = InPlayerController;
	OnPlayerControllerBound(BoundPlayerController.Get());
	
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

void ULimenBaseHudWidget::PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BoundPawn = NewPawn;
	PawnBoundUpdated(BoundPawn.Get());
}
