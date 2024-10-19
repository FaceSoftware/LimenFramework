// Copyright Face Software. All Rights Reserved.


#include "PlayerControllers/LimenMenuPlayerController.h"

#include "HUDs/LimenBaseHUD.h"
#include "HUDs/LimenMenuHud.h"
#include "Kismet/KismetSystemLibrary.h"

ALimenMenuPlayerController::ALimenMenuPlayerController()
{
}

void ALimenMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ALimenMenuPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	SetUIInput();
}

void ALimenMenuPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (CreateHudReference())
	{
		MenuHud->UpdateWidgets(this, GetPawn());

		if (MenuHud->GetMainMenuWidget())
		{
			MenuHud->GetMainMenuWidget()->OnNewGameRequested.AddUObject(this, &ThisClass::RequestNewGame);
			MenuHud->GetMainMenuWidget()->OnLoadGameRequested.AddUObject(this, &ThisClass::RequestLoadGame);
			MenuHud->GetMainMenuWidget()->OnQuitRequested.AddUObject(this, &ThisClass::QuitGame);
		}
	}
}

void ALimenMenuPlayerController::RequestNewGame(APlayerController* InPlayerController, ULimenGameParameters* Params)
{
	check(InPlayerController == this)
	OnNewGameRequested.Broadcast(this, Params);
}

void ALimenMenuPlayerController::RequestLoadGame(APlayerController* InPlayerController, ULimenSaveData* SaveGame)
{
	check(InPlayerController == this)
	OnLoadGameRequested.Broadcast(this, SaveGame);
}

void ALimenMenuPlayerController::QuitGame(APlayerController* InPlayerController)
{
	check(InPlayerController == this)
	UKismetSystemLibrary::QuitGame(GetWorld(), this, EQuitPreference::Quit, false);
}

void ALimenMenuPlayerController::UnbindPawnDelegates(APawn* InPawn)
{
}

void ALimenMenuPlayerController::BindWidgetDelegates()
{
}

void ALimenMenuPlayerController::UnbindWidgetDelegates()
{
}

void ALimenMenuPlayerController::RequestNotificationPause(const bool bWantsToPause)
{
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
