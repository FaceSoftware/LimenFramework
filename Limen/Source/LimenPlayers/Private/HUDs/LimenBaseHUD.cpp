// Copyright Face Software. All Rights Reserved.


#include "HUDs/LimenBaseHUD.h"

#include "Widgets/LimenWidget.h"


ALimenBaseHUD::ALimenBaseHUD() : Super()
{
	PrimaryActorTick.bTickEvenWhenPaused = true;
	
	bAreWidgetsInitialized = false;
	SetTickableWhenPaused(true);	
}

void ALimenBaseHUD::HideActiveWidget()
{
	if (!ActiveWidget)
	{
		return;
	}
	
	HideWidget_Internal(ActiveWidget.Get());
}

UUserWidget* ALimenBaseHUD::GetActiveWidget() const
{
	return ActiveWidget.Get();
}

bool ALimenBaseHUD::InitializeWidget(const TSubclassOf<UUserWidget>& WidgetClass)
{
	return WidgetClass.Get() != nullptr;
}

bool ALimenBaseHUD::CanSwitchWidgetsVisibility() const
{
	return true;
}

void ALimenBaseHUD::ShowWidget_Internal(ULimenWidget* Widget)
{
	check(IsValid(Widget))
	
	if (!CanSwitchWidgetsVisibility())
	{
		return;
	}
	
	Widget->ShowWidget();	
	ActiveWidget = Widget;
}

void ALimenBaseHUD::HideWidget_Internal(ULimenWidget* Widget)
{
	check(IsValid(Widget))
	
	if (!CanSwitchWidgetsVisibility())
	{
		return;
	}

	if (Widget->IsShowing())
	{
		Widget->HideWidget();
	}
	
	ActiveWidget = nullptr;
}
