// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenMainMenuWidget.h"

#include "DataAssets/LimenGameParameters.h"
#include "Subsystems/LimenModalsSubsystem.h"


void ULimenMainMenuWidget::StartNewGame(TSoftObjectPtr<ULimenGameParameters> GameParams)
{
	OnNewGameRequested.Broadcast(BoundPlayerController.Get(), GameParams.LoadSynchronous());
}

void ULimenMainMenuWidget::StartLoadGame(ULimenSaveData* SaveGame)
{
	OnLoadGameRequested.Broadcast(BoundPlayerController.Get(), SaveGame);
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

void ULimenMainMenuWidget::QuitGameModalDismissed(const bool bAccepted)
{
	if (bAccepted)
	{
		OnQuitRequested.Broadcast(BoundPlayerController.Get());
	}
}
