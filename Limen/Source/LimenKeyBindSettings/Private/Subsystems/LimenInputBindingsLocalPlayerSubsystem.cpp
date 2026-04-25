// Copyright FaceSoftware. All rights reserved.


#include "Subsystems/LimenInputBindingsLocalPlayerSubsystem.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenKeyBindSubsystem.h"


ULimenInputBindingsLocalPlayerSubsystem::ULimenInputBindingsLocalPlayerSubsystem()
{
	PawnMappingContextPriority = 0;
	PlayerControllerMappingContextPriority = 1;
}

void ULimenInputBindingsLocalPlayerSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	Super::PlayerControllerChanged(NewPlayerController);

	OldPlayerController = CurrentPlayerController;
	CurrentPlayerController = NewPlayerController;
	UpdateControllerBindings(CurrentPlayerController.Get());
}

void ULimenInputBindingsLocalPlayerSubsystem::AddPawnMappingContext()
{
	if (!GetLocalPlayerChecked()->PlayerController) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem) return;

	const ULimenKeyBindSubsystem* KeyBindSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	if (!KeyBindSubsystem) return;

	const UInputMappingContext* MappingContext = KeyBindSubsystem->GetPawnInputMappingContext(GetLocalPlayerChecked()->PlayerController->GetPawn().Get());
	if (!MappingContext) return;

	InputSystem->AddMappingContext(MappingContext, PawnMappingContextPriority);
}

void ULimenInputBindingsLocalPlayerSubsystem::RemovePawnMappingContext()
{
	if (!GetLocalPlayerChecked()->PlayerController) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem) return;

	const ULimenKeyBindSubsystem* KeyBindSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	if (!KeyBindSubsystem) return;

	const UInputMappingContext* MappingContext = KeyBindSubsystem->GetPawnInputMappingContext(GetLocalPlayerChecked()->PlayerController->GetPawn().Get());
	if (!MappingContext) return;

	InputSystem->RemoveMappingContext(MappingContext);
}

void ULimenInputBindingsLocalPlayerSubsystem::UpdateControllerBindings(APlayerController* PC)
{
	if (!PC) return;

	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!InputSystem) return;
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
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

		// NewPawnDelegateHandle = PC->GetOnNewPawnNotifier().AddUObject(this, &ThisClass::NewPawnNotify);
		// NewPawnNotify(PC->GetPawn().Get());

		PC->OnPossessedPawnChanged.AddUniqueDynamic(this, &ThisClass::PossessedPawnChanged);
		PossessedPawnChanged(nullptr, PC->GetPawn().Get());
	}
}

void ULimenInputBindingsLocalPlayerSubsystem::InputBindUpdated(const FEnhancedActionKeyMapping& ActionKeyMapping)
{
	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	FModifyContextOptions ContextOptions;
	ContextOptions.bForceImmediately = false;
	ContextOptions.bNotifyUserSettings = false;
	ContextOptions.bIgnoreAllPressedKeysUntilRelease = false;

	InputSystem->RequestRebuildControlMappings(ContextOptions, EInputMappingRebuildType::RebuildWithFlush);
}

void ULimenInputBindingsLocalPlayerSubsystem::NewPawnNotify(APawn* NewPawn)
{
	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayerChecked()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (InputSystem == nullptr) return;
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	if (KeyBindSubsystem == nullptr) return;

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

void ULimenInputBindingsLocalPlayerSubsystem::PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	NewPawnNotify(NewPawn);
}
