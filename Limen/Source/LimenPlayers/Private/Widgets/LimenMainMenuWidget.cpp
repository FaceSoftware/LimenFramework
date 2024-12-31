// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenMainMenuWidget.h"

#include "Subsystems/LimenModalsSubsystem.h"


void ULimenMainMenuWidget::StartNewGame()
{
	OnNewGameRequested.Broadcast(BoundPlayerController.Get());
}

void ULimenMainMenuWidget::StartLoadGame(const ULimenSaveData* SaveData)
{
	OnLoadGameRequested.Broadcast(BoundPlayerController.Get(), SaveData);
}

void ULimenMainMenuWidget::OptionsButtonPressed()
{
	OnOptionsRequested.Broadcast(BoundPlayerController.Get());
}

void ULimenMainMenuWidget::QuitButtonPressed()
{
	FModalParams QuitModalParams;
	QuitModalParams.Title = FText::FromString(TEXT("Quit Game"));
	QuitModalParams.Content = FText::FromString(TEXT("Are you sure you want to quit the game?"));
	UConsentModalAsyncAction* ConsentModalAsyncAction = UConsentModalAsyncAction::DisplayConsentModal(this, QuitModalParams);
	ConsentModalAsyncAction->OnModalDismissed.AddUniqueDynamic(this, &ThisClass::QuitGameModalDismissed);
	ConsentModalAsyncAction->Activate();
}

void ULimenMainMenuWidget::QuitGameModalDismissed(ULimenGenericModalWidget* ULimenGenericModalWidget, const bool bAccepted)
{
	if (bAccepted)
	{
		OnQuitRequested.Broadcast(BoundPlayerController.Get());
	}
}
