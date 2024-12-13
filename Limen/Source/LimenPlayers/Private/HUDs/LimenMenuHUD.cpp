// Copyright Face Software. All Rights Reserved.


#include "HUDs/LimenMenuHUD.h"

#include "Widgets/LimenMainMenuWidget.h"
#include "Widgets/LimenWidget.h"

ALimenMenuHUD::ALimenMenuHUD()
{
	MainMenuWidget = nullptr;
	OptionsWidget = nullptr;
}

void ALimenMenuHUD::ShowMainMenuWidget()
{
	if (MainMenuWidget)
	{
		ShowWidget_Internal(MainMenuWidget.Get());
	}
}

void ALimenMenuHUD::HideMainMenuWidget()
{
	if (MainMenuWidget)
	{
		HideWidget_Internal(MainMenuWidget.Get());
	}
}

bool ALimenMenuHUD::IsMainMenuWidgetShowing() const
{
	if (!MainMenuWidget)
		return false;
	
	return MainMenuWidget->IsShowing();
}

ULimenMainMenuWidget* ALimenMenuHUD::GetMainMenuWidget() const
{
	return MainMenuWidget.Get();
}

void ALimenMenuHUD::ShowOptionsWidget()
{
	if (OptionsWidget)
	{
		ShowWidget_Internal(OptionsWidget.Get());
	}
}

void ALimenMenuHUD::HideOptionsWidget()
{
	if (OptionsWidget)
	{
		HideWidget_Internal(OptionsWidget.Get());
	}
}

bool ALimenMenuHUD::IsOptionsWidgetShowing() const
{
	if (!OptionsWidget)
		return false;
	
	return OptionsWidget->IsShowing();
}

ULimenMenuWidget* ALimenMenuHUD::GetOptionsWidget() const
{
	return OptionsWidget.Get();
}

const TMap<int32, UTexture2D*>& ALimenMenuHUD::GetLevelContextBackgrounds() const
{
	return LevelContextBackgrounds;
}

void ALimenMenuHUD::DestroyWidgets()
{
	Super::DestroyWidgets();
	
	if (MainMenuWidget)
	{
		MainMenuWidget->DestroyWidget(false);
		MainMenuWidget = nullptr;
	}

	if (OptionsWidget)
	{
		OptionsWidget->DestroyWidget(false);
		OptionsWidget = nullptr;
	}
}

void ALimenMenuHUD::InitializeWidgets()
{
	Super::InitializeWidgets();
	
	if (!MainMenuWidgetClass.IsNull())
	{
		MainMenuWidget = ULimenWidget::IsWidgetValid<ULimenMainMenuWidget>(
			MainMenuWidget.Get(), GetOwningPlayerController(), MainMenuWidgetClass.LoadSynchronous(), true);

		MainMenuWidget->BindPlayerController(GetOwningPlayerController());
		MainMenuWidget->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnMainMenuVisibilityChanged);
	}

	if (!OptionsWidgetClass.IsNull())
	{
		OptionsWidget = ULimenWidget::IsWidgetValid<ULimenMenuWidget>(
			OptionsWidget.Get(), GetOwningPlayerController(), OptionsWidgetClass.LoadSynchronous(), true);

		OptionsWidget->BindPlayerController(GetOwningPlayerController());
		OptionsWidget->OnLimenVisibilityChanged.AddUniqueDynamic(this, &ThisClass::OnOptionsMenuVisibilityChanged);
	}
}

void ALimenMenuHUD::OnMainMenuVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		HideOptionsWidget();
	}
	else
	{
		
	}
}

void ALimenMenuHUD::OnOptionsMenuVisibilityChanged(const bool bIsVisible)
{
	if (bIsVisible)
	{
		ShowMainMenuWidget();
	}
	else
	{
		
	}
}
