// Fill out your copyright notice in the Description page of Project Settings.


#include "LocalPlayer/LimenLocalPlayer.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenKeyBindSubsystem.h"


void ULimenLocalPlayer::ReceivedPlayerController(APlayerController* NewController)
{
	OldPlayerController = CurrentPlayerController;
	CurrentPlayerController = NewController;
	UpdateControllerBindings(CurrentPlayerController.Get());

	Super::ReceivedPlayerController(NewController);
}

void ULimenLocalPlayer::UpdateControllerBindings(APlayerController* PC)
{
	check(PC)

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSystem != nullptr)
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	check(KeyBindSubsystem != nullptr)

	if (const UInputMappingContext* PlayerMappings = KeyBindSubsystem->GetPlayerInputMappingContext(PC);
		PlayerMappings != nullptr)
	{
		InputSystem->AddMappingContext(PlayerMappings, 1);
		for (auto& Mapping : PlayerMappings->GetMappings())
		{
			InputBindUpdated(Mapping);
		}

		if (!KeyBindSubsystem->OnKeyBindUpdate.IsBoundToObject(this))
		{
			KeyBindSubsystem->OnKeyBindUpdate.AddUObject(this, &ThisClass::InputBindUpdated);
		}
	}

	if (OldPlayerController.IsValid())
	{
		OldPlayerController->GetOnNewPawnNotifier().Remove(NewPawnDelegateHandle);
		NewPawnDelegateHandle.Reset();
	}

	NewPawnDelegateHandle = PC->GetOnNewPawnNotifier().AddUObject(this, &ThisClass::NewPawnSet);
	NewPawnSet(PC->GetPawn());
}

void ULimenLocalPlayer::InputBindUpdated(const FEnhancedActionKeyMapping& ActionKeyMapping)
{
	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	FModifyContextOptions ContextOptions;
	ContextOptions.bForceImmediately = false;
	ContextOptions.bNotifyUserSettings = false;
	ContextOptions.bIgnoreAllPressedKeysUntilRelease = false;

	InputSystem->RequestRebuildControlMappings(ContextOptions, EInputMappingRebuildType::RebuildWithFlush);
}

void ULimenLocalPlayer::NewPawnSet(APawn* NewPawn)
{
	if (!NewPawn)
	{
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSystem != nullptr)
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	check(KeyBindSubsystem != nullptr)

	if (const UInputMappingContext* PlayerMappings = KeyBindSubsystem->GetPawnInputMappingContext(NewPawn);
		PlayerMappings != nullptr)
	{
		InputSystem->AddMappingContext(PlayerMappings, 1);
		for (auto& Mapping : PlayerMappings->GetMappings())
		{
			InputBindUpdated(Mapping);
		}
		
		if (!KeyBindSubsystem->OnKeyBindUpdate.IsBoundToObject(this))
		{
			KeyBindSubsystem->OnKeyBindUpdate.AddUObject(this, &ThisClass::InputBindUpdated);
		}
	}
}
