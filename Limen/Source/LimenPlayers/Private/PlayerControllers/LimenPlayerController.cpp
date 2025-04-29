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
#include "Widgets/LimenPauseMenuWidget.h"


ALimenPlayerController::ALimenPlayerController(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	bEnableMouseOverEvents = true;
	bEnableClickEvents = true;
}

void ALimenPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetGameInput();
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

void ALimenPlayerController::ClientSetHUD_Implementation(TSubclassOf<AHUD> NewHUDClass)
{
	Super::ClientSetHUD_Implementation(NewHUDClass);
	LimenHUD = Cast<ALimenHUD>(GetHUD());
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
		verify(LimenHUD.IsValid())

		if (LimenHUD->IsCharacterHudShowing() || LimenHUD->GetActiveWidget() == nullptr)
		{
			LimenHUD->TogglePauseMenuWidget();
		}
		else if (LimenHUD->GetActiveWidget() != nullptr)
		{
			LimenHUD->HideActiveWidget();
		}
	}
}

void ALimenPlayerController::OnPawnDeath(ULimenAttributeBase* Attribute, const float Health)
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
		LimenHUD->GetGameMenuWidget()->OnLimenAnimationFinished.AddUniqueDynamic(this, &ThisClass::GameMenuAnimationFinished);
		LimenHUD->GetGameMenuWidget()->OnItemActionRequest.AddUniqueDynamic(this, &ThisClass::HandleItemActionRequest);
	}

	if (LimenHUD->GetPauseMenuWidget())
	{
		LimenHUD->GetPauseMenuWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::PauseMenuVisibilityChanged);
		LimenHUD->GetPauseMenuWidget()->OnLimenAnimationFinished.AddUniqueDynamic(this, &ThisClass::PauseMenuAnimationFinished);
	}

	if (LimenHUD->GetHudWidget())
	{
		LimenHUD->GetHudWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::HUDVisibilityChanged);
		LimenHUD->GetHudWidget()->OnLimenAnimationFinished.AddUniqueDynamic(this, &ThisClass::HUDAnimationFinished);
	}

	if (LimenHUD->GetDeathScreenWidget())
	{
		LimenHUD->GetDeathScreenWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::DeathScreenVisibilityChanged);
		LimenHUD->GetDeathScreenWidget()->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::DeathScreenAnimationFinished);
	}
}

void ALimenPlayerController::UnbindWidgetDelegates()
{
	if (LimenHUD->GetGameMenuWidget())
	{
		LimenHUD->GetGameMenuWidget()->OnLimenVisibilityChanged.RemoveAll(this);
		LimenHUD->GetGameMenuWidget()->OnLimenAnimationFinished.RemoveAll(this);
	}

	if (LimenHUD->GetPauseMenuWidget())
	{
		LimenHUD->GetPauseMenuWidget()->OnLimenVisibilityChanged.RemoveAll(this);
		LimenHUD->GetPauseMenuWidget()->OnLimenAnimationFinished.RemoveAll(this);
	}

	if (LimenHUD->GetHudWidget())
	{
		LimenHUD->GetHudWidget()->OnLimenVisibilityChanged.RemoveAll(this);
		LimenHUD->GetHudWidget()->OnLimenAnimationFinished.RemoveAll(this);
	}

	if (LimenHUD->GetDeathScreenWidget())
	{
		LimenHUD->GetDeathScreenWidget()->OnLimenVisibilityChanged.RemoveAll(this);
		LimenHUD->GetDeathScreenWidget()->OnLimenAnimationFinished.RemoveAll(this);
	}

	if (LimenHUD->GetItemSmithWidget())
	{
		LimenHUD->GetItemSmithWidget()->OnLimenVisibilityChanged.RemoveAll(this);
		LimenHUD->GetItemSmithWidget()->OnLimenAnimationFinished.RemoveAll(this);
	}
}

void ALimenPlayerController::LoadingScreenVisibilityChanged(const bool bIsVisible)
{
	Super::LoadingScreenVisibilityChanged(bIsVisible);
}

void ALimenPlayerController::GameMenuVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		RequestPause(EPauseReason::GameMenu);
	}
	else
	{
		SetGameInput();
	}
}

void ALimenPlayerController::GameMenuAnimationFinished(const bool bIsVisible)
{
	if (bIsVisible)
	{
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
	}
	else
	{
		SetGameInput();
	}
}

void ALimenPlayerController::PauseMenuAnimationFinished(const bool bIsVisible)
{
	if (bIsVisible)
	{
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
	}
	else
	{
		SetGameInput();
	}
}

void ALimenPlayerController::HUDAnimationFinished(const bool bIsVisible)
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
	}
	else
	{
		SetGameInput();
	}
}

void ALimenPlayerController::DeathScreenAnimationFinished(const bool bIsVisible)
{
	if (bIsVisible)
	{
		SetGameInput();
	}
	else
	{
	}
}
