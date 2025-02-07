// Copyright Face Software. All Rights Reserved.


#include "PlayerControllers/LimenPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "Attributes/LimenHealthAttribute.h"
#include "Characters/LimenPlayerCharacter.h"
#include "Components/LimenAbilityComponent.h"
#include "Engine/GameInstance.h"
#include "HUDs/LimenHUD.h"
#include "Subsystems/LimenKeyBindSubsystem.h"
#include "Subsystems/LimenLevelTransitionSubsystem.h"
#include "Widgets/LimenDeathScreen.h"
#include "Widgets/LimenGameMenuWrapperWidget.h"
#include "Widgets/LimenHudWidget.h"
#include "Widgets/LimenItemSmithWrapperWidget.h"
#include "UMG/LimenLoadingScreenWidget.h"
#include "Widgets/LimenPauseMenuWidget.h"


ALimenPlayerController::ALimenPlayerController(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
}

void ALimenPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	const ULocalPlayer* LocalPlayer = Cast<ULocalPlayer>(Player);
	UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (ULimenKeyBindSubsystem* KeyBindSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>(); KeyBindSubsystem != nullptr)
	{
		if (const UInputMappingContext* PlayerMappings = KeyBindSubsystem->GetPlayerInputMappingContext(this);
			PlayerMappings != nullptr)
		{
			InputSystem->AddMappingContext(PlayerMappings, 1);
		}
		
		KeyBindSubsystem->OnKeyBindUpdate.AddUObject(this, &ALimenPlayerController::InputBindUpdated);
	}

	auto* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent.Get());
	EnhancedInput->BindAction(PauseMenuInputAction.LoadSynchronous(), ETriggerEvent::Triggered, this, &ThisClass::PauseInput);
}

void ALimenPlayerController::OnPossess(APawn* InPawn)
{
	if (InPawn != nullptr)
	{
		LimenPlayerCharacter = Cast<ALimenPlayerCharacter>(InPawn);
	}
	
	Super::OnPossess(InPawn);
	
	if (const ULimenLevelTransitionSubsystem* LevelTransitionSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenLevelTransitionSubsystem>();
		LevelTransitionSubsystem != nullptr && !LevelTransitionSubsystem->IsLoadingScreenActive())
	{
		LoadingScreenVisibilityChanged(false);
	}
}

void ALimenPlayerController::OnUnPossess()
{
	LimenPlayerCharacter = nullptr;
	Super::OnUnPossess();
}

ALimenPlayerCharacter* ALimenPlayerController::GetLimenCharacter() const
{
	return LimenPlayerCharacter.Get();
}

void ALimenPlayerController::HandleItemActionRequest(ULimenItemAction* ActionRequested)
{
	check(ActionRequested != nullptr);
	check(GetLimenCharacter() != nullptr);
	GetLimenCharacter()->HandleItemActionRequests(ActionRequested);
}

void ALimenPlayerController::InputBindUpdated(const FEnhancedActionKeyMapping& ActionKeyMapping)
{
	UEnhancedInputLocalPlayerSubsystem* InputSystem = GetLocalPlayer()->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	InputSystem->RequestRebuildControlMappings();
}

void ALimenPlayerController::PauseInput(const FInputActionInstance& Instance)
{	
	if (Instance.GetValue().Get<bool>())
	{
		verify(CreateHudReference())

		if (LimenHUD->IsCharacterHudShowing())
		{
			LimenHUD->TogglePauseMenuWidget();
		}
		else // Is not seeing the hud, probably inside a menu
		{
			LimenHUD->HideActiveWidget();
		}
		
	}
}

bool ALimenPlayerController::CreateHudReference()
{
	Super::CreateHudReference();

	if (LimenHUD != nullptr)
	{
		return true;
	}
	
	LimenHUD = Cast<ALimenHUD>(GetHUD());
	return LimenHUD != nullptr;
}

void ALimenPlayerController::OnPawnDeath(const float Health)
{	
	if (LimenHUD.IsValid() && LimenHUD->GetDeathScreenWidget())
	{
		// UnPossess and show the widget at the same time
		LimenHUD->ShowDeathScreenWidget();
		UnPossess();
	}
}

void ALimenPlayerController::BindPawnDelegates(APawn* NewPawn)
{
	if (NewPawn != GetPawn())
	{
		return;
	}
	
	ULimenAbilityComponent* AbilityComponent = NewPawn->GetComponentByClass<ULimenAbilityComponent>();
	if (AbilityComponent == nullptr)
	{
		return;
	}

	ULimenHealthAttribute* HealthAttribute = AbilityComponent->GetAttribute<ULimenHealthAttribute>();
	if (HealthAttribute == nullptr)
	{
		return;
	}

	HealthAttribute->OnAttributeEmpty.AddUniqueDynamic(this, &ThisClass::OnPawnDeath);
}

void ALimenPlayerController::UnbindPawnDelegates(APawn* InPawn)
{
	if (InPawn == nullptr || InPawn != LimenPlayerCharacter)
	{
		return;
	}

	ULimenAbilityComponent* AbilityComponent = InPawn->GetComponentByClass<ULimenAbilityComponent>();
	if (AbilityComponent == nullptr)
	{
		return;
	}

	ULimenHealthAttribute* HealthAttribute = AbilityComponent->GetAttribute<ULimenHealthAttribute>();
	if (HealthAttribute == nullptr)
	{
		return;
	}
	
	HealthAttribute->OnAttributeEmpty.RemoveAll(this);
}

void ALimenPlayerController::BindWidgetDelegates()
{
	if (LimenHUD->GetGameMenuWidget())
	{
		LimenHUD->GetGameMenuWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::GameMenuVisibilityChanged);
		LimenHUD->GetGameMenuWidget()->OnItemActionRequest.AddUniqueDynamic(this, &ThisClass::HandleItemActionRequest);
	}

	if (LimenHUD->GetPauseMenuWidget())
	{
		LimenHUD->GetPauseMenuWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::PauseMenuVisibilityChanged);
	}

	if (LimenHUD->GetHudWidget())
	{
		LimenHUD->GetHudWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::HUDVisibilityChanged);
	}

	if (LimenHUD->GetDeathScreenWidget())
	{
		LimenHUD->GetDeathScreenWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::DeathScreenVisibilityChanged);
	}

	if (LimenHUD->GetItemSmithWidget())
	{
		LimenHUD->GetItemSmithWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::MainMenuVisibilityChanged);
	}
}

void ALimenPlayerController::UnbindWidgetDelegates()
{
	if (LimenHUD->GetGameMenuWidget())
	{
		LimenHUD->GetGameMenuWidget()->OnLimenVisibilityChanged.RemoveAll(this);
	}

	if (LimenHUD->GetPauseMenuWidget())
	{
		LimenHUD->GetPauseMenuWidget()->OnLimenVisibilityChanged.RemoveAll(this);
	}

	if (LimenHUD->GetHudWidget())
	{
		LimenHUD->GetHudWidget()->OnLimenVisibilityChanged.RemoveAll(this);
	}

	if (LimenHUD->GetDeathScreenWidget())
	{
		LimenHUD->GetDeathScreenWidget()->OnLimenVisibilityChanged.RemoveAll(this);
	}

	if (LimenHUD->GetItemSmithWidget())
	{
		LimenHUD->GetItemSmithWidget()->OnLimenVisibilityChanged.RemoveAll(this);
	}
}

void ALimenPlayerController::LoadingScreenVisibilityChanged(const bool bIsVisible)
{
	Super::LoadingScreenVisibilityChanged(bIsVisible);
	
	if (bIsVisible)
	{
		if (CreateHudReference())
		{
			LimenHUD->HideCharacterHudWidget();
		}
		
		SetUIOnlyInput();
	}
	else
	{
		if (CreateHudReference())
		{
			LimenHUD->ShowCharacterHudWidget();
		}
		
		SetGameInput();
	}
}

void ALimenPlayerController::GameMenuVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		RequestPause(EPauseReason::GameMenu);
		SetUIInput();
	}
	else
	{
		RequestUnPause();
	}
}

void ALimenPlayerController::PauseMenuVisibilityChanged(const bool bIsVisible)
{	
	if (bIsVisible)
	{
		RequestPause(EPauseReason::PauseMenu);
		SetUIInput();
	}
	else
	{
		RequestUnPause();
	}
}

void ALimenPlayerController::HUDVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		SetGameInput();
	}
	else
	{
		
	}
}

void ALimenPlayerController::DeathScreenVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		SetUIInput();
	}
	else
	{
		
	}
}

void ALimenPlayerController::MainMenuVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		SetUIInput();
		RequestPause(EPauseReason::GameMenu);
	}
	else
	{
		RequestUnPause();
	}
}
