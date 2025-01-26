// Copyright Face Software. All Rights Reserved.


#include "PlayerControllers/LimenMenuPlayerController.h"

#include "HUDs/LimenMenuHud.h"
#include "Kismet/KismetSystemLibrary.h"


ALimenMenuPlayerController::ALimenMenuPlayerController()
{
}

void ALimenMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	SetUIInput();
}

void ALimenMenuPlayerController::BindWidgetDelegates()
{
	if (MenuHud->GetMainMenuWidget())
	{
		MenuHud->GetMainMenuWidget()->OnNewGameRequested.AddUObject(this, &ThisClass::RequestNewGame);
		MenuHud->GetMainMenuWidget()->OnLoadGameRequested.AddUObject(this, &ThisClass::RequestLoadGame);
		MenuHud->GetMainMenuWidget()->OnQuitRequested.AddUObject(this, &ThisClass::QuitGame);
	}
}

void ALimenMenuPlayerController::UnbindWidgetDelegates()
{
	if (MenuHud->GetMainMenuWidget())
	{
		MenuHud->GetMainMenuWidget()->OnNewGameRequested.RemoveAll(this);
		MenuHud->GetMainMenuWidget()->OnLoadGameRequested.RemoveAll(this);
		MenuHud->GetMainMenuWidget()->OnQuitRequested.RemoveAll(this);
	}
}

void ALimenMenuPlayerController::LoadingScreenVisibilityChanged(const bool bIsVisible)
{
	Super::LoadingScreenVisibilityChanged(bIsVisible);

	if (bIsVisible)
	{
	}
	else
	{
		SetUIInput();
	}
}

bool ALimenMenuPlayerController::CreateHudReference()
{
	Super::CreateHudReference();

	if (MenuHud.IsValid())
	{
		return true;
	}

	MenuHud = Cast<ALimenMenuHUD>(GetHUD());
	return MenuHud.IsValid();
}

void ALimenMenuPlayerController::RequestNewGame(APlayerController* InPlayerController)
{
	check(InPlayerController == this);
	OnNewGameRequested.Broadcast(this);
}

void ALimenMenuPlayerController::RequestLoadGame(APlayerController* InPlayerController, const ULimenSaveData* SaveGame)
{
	check(InPlayerController == this);
	OnLoadGameRequested.Broadcast(this, SaveGame);
}

void ALimenMenuPlayerController::QuitGame(APlayerController* InPlayerController)
{
	check(InPlayerController == this);
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}
