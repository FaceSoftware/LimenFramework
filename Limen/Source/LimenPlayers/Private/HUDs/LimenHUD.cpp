// Copyright Face Software. All Rights Reserved.


#include "HUDs/LimenHUD.h"

#include "Widgets/LimenDeathScreen.h"
#include "Widgets/LimenGameMenuWrapperWidget.h"
#include "Widgets/LimenHudWidget.h"
#include "Widgets/LimenItemSmithWrapperWidget.h"
#include "Widgets/LimenPauseMenuWidget.h"


ALimenHUD::ALimenHUD()
{
	bPostProcessEnabled = true;
}

void ALimenHUD::ShowGameMenuWidget()
{
	if (GameMenuWidget)
	{
		ShowWidget_Internal(GameMenuWidget.Get());
	}
}

void ALimenHUD::HideGameMenuWidget()
{
	if (GameMenuWidget)
	{
		HideWidget_Internal(GameMenuWidget.Get());
	}
}

void ALimenHUD::ToggleGameMenuWidget()
{
	IsGameMenuShowing() ? HideGameMenuWidget() : ShowGameMenuWidget();
}

bool ALimenHUD::IsGameMenuShowing() const
{
	if (!GameMenuWidget)
		return false;
	
	return GameMenuWidget->IsShowing();
}

ULimenGameMenuWrapperWidget* ALimenHUD::GetGameMenuWidget() const
{
	return GameMenuWidget.Get();
}

void ALimenHUD::ShowCharacterHudWidget()
{
	if (CharacterHudWidget && !bForceHUDState)
	{
		ShowWidget_Internal(CharacterHudWidget.Get());
	}
}

void ALimenHUD::HideCharacterHudWidget()
{
	if (CharacterHudWidget && !bForceHUDState)
	{
		HideWidget_Internal(CharacterHudWidget.Get());
	}
}

void ALimenHUD::ForceHUDState(const bool bForce)
{
	bForceHUDState = bForce;
}

void ALimenHUD::ToggleCharacterHudWidget()
{
	IsCharacterHudShowing() ? HideCharacterHudWidget() : ShowCharacterHudWidget();
}

bool ALimenHUD::IsCharacterHudShowing() const
{
	if (CharacterHudWidget == nullptr) return false;
	
	return CharacterHudWidget->IsShowing();
}

ULimenHudWidget* ALimenHUD::GetHudWidget() const
{
	return CharacterHudWidget.Get();
}

void ALimenHUD::ShowPauseMenuWidget()
{
	if (PauseMenuWidget)
	{
		ShowWidget_Internal(PauseMenuWidget.Get());
	}
}

void ALimenHUD::HidePauseMenuWidget()
{
	if (PauseMenuWidget)
	{
		HideWidget_Internal(PauseMenuWidget.Get());
	}
}

void ALimenHUD::TogglePauseMenuWidget()
{
	IsPauseMenuShowing() ? HidePauseMenuWidget() : ShowPauseMenuWidget();
}

bool ALimenHUD::IsPauseMenuShowing() const
{
	if (!PauseMenuWidget)
		return false;
	
	return PauseMenuWidget->IsShowing();
}

ULimenPauseMenuWidget* ALimenHUD::GetPauseMenuWidget() const
{
	return PauseMenuWidget.Get();
}

void ALimenHUD::ShowDeathScreenWidget()
{
	if (DeathScreenWidget)
	{
		ShowWidget_Internal(DeathScreenWidget.Get());
	}
}

void ALimenHUD::HideDeathScreenWidget()
{
	if (DeathScreenWidget)
	{
		HideWidget_Internal(DeathScreenWidget.Get());
	}
}

void ALimenHUD::ToggleDeathScreenWidget()
{
	IsDeathScreenShowing() ? HideDeathScreenWidget() : ShowDeathScreenWidget();
}

bool ALimenHUD::IsDeathScreenShowing() const
{
	return DeathScreenWidget->IsShowing();
}

ULimenDeathScreen* ALimenHUD::GetDeathScreenWidget() const
{
	return DeathScreenWidget.Get();
}

void ALimenHUD::ShowItemSmithWidget()
{
	if (ItemSmithWrapperWidget)
	{
		ShowWidget_Internal(ItemSmithWrapperWidget.Get());
	}
}

void ALimenHUD::HideItemSmithWidget()
{
	if (ItemSmithWrapperWidget)
	{
		HideWidget_Internal(ItemSmithWrapperWidget.Get());
	}
}

void ALimenHUD::ToggleItemSmithWidget()
{
	IsItemSmithShowing() ? HideItemSmithWidget() : ShowItemSmithWidget();
}

bool ALimenHUD::IsItemSmithShowing() const
{
	return ItemSmithWrapperWidget->IsShowing();
}

ULimenItemSmithWrapperWidget* ALimenHUD::GetItemSmithWidget() const
{
	return ItemSmithWrapperWidget.Get();
}

void ALimenHUD::QueueNotification(const FNotificationParams& InParams)
{
	Super::QueueNotification(InParams);
}

void ALimenHUD::SetPostProcessEnabled(const bool bEnable)
{
	if (bPostProcessEnabled == bEnable)
	{
		return;
	}

	bPostProcessEnabled = !bPostProcessEnabled;
	if (bPostProcessEnabled)
	{
		OnPostProcessEnabled();
	}
	else
	{
		OnPostProcessDisabled();
	}
}

bool ALimenHUD::IsPostProcessEnabled() const
{
	return bPostProcessEnabled;
}

void ALimenHUD::DestroyWidgets()
{
	Super::DestroyWidgets();
	
	if (CharacterHudWidget)
	{
		CharacterHudWidget->DestroyWidget(false);
		CharacterHudWidget = nullptr;
	}
	if (GameMenuWidget)
	{
		GameMenuWidget->DestroyWidget(false);
		GameMenuWidget = nullptr;
	}
	if (PauseMenuWidget)
	{
		PauseMenuWidget->DestroyWidget(false);
		PauseMenuWidget = nullptr;
	}
}

void ALimenHUD::InitializeWidgets()
{
	Super::InitializeWidgets();
	
	if (!GameMenuWidgetClass.IsNull())
	{
		GameMenuWidget = ULimenWidget::IsWidgetValid<ULimenGameMenuWrapperWidget>(
			GameMenuWidget.Get(), GetOwningPlayerController(), GameMenuWidgetClass.LoadSynchronous(), true);

		GameMenuWidget->BindPlayerController(GetOwningPlayerController());
		GameMenuWidget->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnGameMenuVisibilityChanged);
	}

	if (!CharacterHudWidgetClass.IsNull())
	{
		CharacterHudWidget = ULimenWidget::IsWidgetValid<ULimenHudWidget>(
			CharacterHudWidget.Get(), GetOwningPlayerController(), CharacterHudWidgetClass.LoadSynchronous(), true);
		
		CharacterHudWidget->BindPawn(GetOwningPlayerController()->GetPawn());
		CharacterHudWidget->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnCharacterHudVisibilityChanged);
	}

	if (!PauseMenuWidgetClass.IsNull())
	{
		PauseMenuWidget = ULimenWidget::IsWidgetValid<ULimenPauseMenuWidget>(
			PauseMenuWidget.Get(), GetOwningPlayerController(), PauseMenuWidgetClass.LoadSynchronous(), true);

		PauseMenuWidget->BindPlayerController(GetOwningPlayerController());
		PauseMenuWidget->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnPauseMenuVisibilityChanged);
	}

	if (!DeathScreenWidgetClass.IsNull())
	{
		DeathScreenWidget = ULimenWidget::IsWidgetValid<ULimenDeathScreen>(
			DeathScreenWidget.Get(), GetOwningPlayerController(), DeathScreenWidgetClass.LoadSynchronous(), true);

		DeathScreenWidget->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnDeathScreenVisibilityChanged);
	}

	if (!ItemSmithWrapperWidgetClass.IsNull())
	{
		ItemSmithWrapperWidget = ULimenWidget::IsWidgetValid<ULimenItemSmithWrapperWidget>(
			ItemSmithWrapperWidget.Get(), GetOwningPlayerController(), ItemSmithWrapperWidgetClass.LoadSynchronous(), true);

		ItemSmithWrapperWidget->BindPlayerController(GetOwningPlayerController());
		ItemSmithWrapperWidget->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnUpgradeShopVisibilityChanged);
	}
}

void ALimenHUD::HudInitialized(APlayerController* PlayerController)
{
	Super::HudInitialized(PlayerController);
	
	if (GameMenuWidget)
	{
		WidgetsList.Push(GameMenuWidget.Get());
	}

	if (PauseMenuWidget)
	{
		WidgetsList.Push(PauseMenuWidget.Get());
	}

	if (CharacterHudWidget)
	{
		WidgetsList.Push(CharacterHudWidget.Get());
	}

	if (DeathScreenWidget)
	{
		WidgetsList.Push(DeathScreenWidget.Get());
	}

	if (ItemSmithWrapperWidget)
	{
		WidgetsList.Push(ItemSmithWrapperWidget.Get());
	}
}

bool ALimenHUD::IsAnyAnimationInProgress() const
{
	for (const auto* Widget : WidgetsList)
	{
		if (Widget->IsAnimating())
		{
			return true;
		}
	}

	return false;
}

bool ALimenHUD::CanSwitchWidgetsVisibility() const
{
	if (!Super::CanSwitchWidgetsVisibility())
	{
		return false;
	}

	return !IsAnyAnimationInProgress();
}

void ALimenHUD::OnLoadingScreenVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		HideCharacterHudWidget();
		HideGameMenuWidget();
		HidePauseMenuWidget();
	}
	else
	{
		ShowCharacterHudWidget();
	}
}

void ALimenHUD::OnCharacterHudVisibilityChanged(const bool bIsVisible)
{
	
}

void ALimenHUD::OnGameMenuVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		HideCharacterHudWidget();
		HidePauseMenuWidget();
	}
	else
	{
		ShowCharacterHudWidget();
	}
}

void ALimenHUD::OnPauseMenuVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		HideCharacterHudWidget();
		HideGameMenuWidget();
	}
	else
	{
		ShowCharacterHudWidget();
	}
}

void ALimenHUD::OnDeathScreenVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		HideCharacterHudWidget();
	}
}

void ALimenHUD::OnUpgradeShopVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		HideCharacterHudWidget();
		HidePauseMenuWidget();
	}
	else
	{
		ShowCharacterHudWidget();
	}
}
