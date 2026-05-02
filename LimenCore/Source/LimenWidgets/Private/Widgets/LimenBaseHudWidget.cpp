// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenBaseHudWidget.h"

#include "Blueprint/WidgetTree.h"
#include "GameFramework/Pawn.h"


ULimenBaseHudWidget::ULimenBaseHudWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetWidgetLevel(1);
	DefaultVisibleState = ESlateVisibility::HitTestInvisible;
	DefaultHiddenState = ESlateVisibility::Collapsed;
}

void ULimenBaseHudWidget::BindPlayerController(APlayerController* InPlayerController)
{
	if (BoundPlayerController.IsValid())
	{
		BoundPlayerController->OnPossessedPawnChanged.RemoveDynamic(this, &ThisClass::PossessedPawnChanged);
	}
	
	BoundPlayerController = InPlayerController;
	PlayerControllerBound(BoundPlayerController.Get());
	
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

APlayerController* ULimenBaseHudWidget::GetBoundPlayerController() const
{
	return BoundPlayerController.Get();
}

void ULimenBaseHudWidget::PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	BoundPawn = NewPawn;
	PawnBoundUpdated(BoundPawn.Get());
}
