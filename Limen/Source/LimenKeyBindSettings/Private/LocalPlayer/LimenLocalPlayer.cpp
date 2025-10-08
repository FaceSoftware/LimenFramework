// Copyright © 2024 FaceSoftware. All rights reserved.


#include "LocalPlayer/LimenLocalPlayer.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenKeyBindSubsystem.h"


ULimenLocalPlayer::ULimenLocalPlayer()
{
	PawnMappingContextPriority = 0;
	PlayerControllerMappingContextPriority = 1;
}

void ULimenLocalPlayer::AddPawnMappingContext()
{
	if (!PlayerController) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem) return;

	const ULimenKeyBindSubsystem* KeyBindSubsystem = GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	if (!KeyBindSubsystem) return;

	const UInputMappingContext* MappingContext = KeyBindSubsystem->GetPawnInputMappingContext(PlayerController->GetPawn().Get());
	if (!MappingContext) return;

	InputSystem->AddMappingContext(MappingContext, PawnMappingContextPriority);
}

void ULimenLocalPlayer::RemovePawnMappingContext()
{
	if (!PlayerController) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem) return;

	const ULimenKeyBindSubsystem* KeyBindSubsystem = GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	if (!KeyBindSubsystem) return;

	const UInputMappingContext* MappingContext = KeyBindSubsystem->GetPawnInputMappingContext(PlayerController->GetPawn().Get());
	if (!MappingContext) return;

	InputSystem->RemoveMappingContext(MappingContext);
}

void ULimenLocalPlayer::ReceivedPlayerController(APlayerController* NewController)
{
	OldPlayerController = CurrentPlayerController;
	CurrentPlayerController = NewController;
	UpdateControllerBindings(CurrentPlayerController.Get());

	Super::ReceivedPlayerController(NewController);
}

void ULimenLocalPlayer::UpdateControllerBindings(APlayerController* PC)
{
	if (!PC) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem) return;
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	if (!KeyBindSubsystem) return;

	if (const UInputMappingContext* PlayerMappings = KeyBindSubsystem->GetPlayerInputMappingContext(PC);
		PlayerMappings != nullptr)
	{
		InputSystem->AddMappingContext(PlayerMappings, PlayerControllerMappingContextPriority);
		for (auto& Mapping : PlayerMappings->GetMappings())
		{
			InputBindUpdated(Mapping);
		}

		if (!KeyBindSubsystem->OnKeyBindUpdate.IsBoundToObject(this))
		{
			KeyBindSubsystem->OnKeyBindUpdate.AddUObject(this, &ThisClass::InputBindUpdated);
		}
	}

	if (OldPlayerController != PC)
	{
		if (OldPlayerController.IsValid())
		{
			OldPlayerController->GetOnNewPawnNotifier().Remove(NewPawnDelegateHandle);
		}

		NewPawnDelegateHandle = PC->GetOnNewPawnNotifier().AddUObject(this, &ThisClass::PossessedPawnChanged);
		PossessedPawnChanged(PC->GetPawn().Get());
	}
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

void ULimenLocalPlayer::PossessedPawnChanged(APawn* NewPawn)
{
	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(InputSystem != nullptr)
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	check(KeyBindSubsystem != nullptr)

	if (const UInputMappingContext* PreviousPawnMappings = KeyBindSubsystem->GetPawnInputMappingContext(CurrentPawn.Get()))
	{
		InputSystem->RemoveMappingContext(PreviousPawnMappings);
	}

	if (const UInputMappingContext* CurrentPawnMappings = KeyBindSubsystem->GetPawnInputMappingContext(NewPawn))
	{
		InputSystem->AddMappingContext(CurrentPawnMappings, PawnMappingContextPriority);
		for (auto& Mapping : CurrentPawnMappings->GetMappings())
		{
			InputBindUpdated(Mapping);
		}
		
		if (!KeyBindSubsystem->OnKeyBindUpdate.IsBoundToObject(this))
		{
			KeyBindSubsystem->OnKeyBindUpdate.AddUObject(this, &ThisClass::InputBindUpdated);
		}
	}

	CurrentPawn = NewPawn;
}
