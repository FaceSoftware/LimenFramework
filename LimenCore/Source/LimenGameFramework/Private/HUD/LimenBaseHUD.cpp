// Copyright Face Software. All Rights Reserved.


#include "HUD/LimenBaseHUD.h"

#include "Components/LimenNotificationComponent.h"
#include "CppClasses/LimenNotification.h"
#include "Widgets/LimenWidget.h"


ALimenBaseHUD::ALimenBaseHUD() : Super()
{
	PrimaryActorTick.bTickEvenWhenPaused = true;
	bAreWidgetsInitialized = false;
	SetTickableWhenPaused(true);
	bForceHideHud = 0;	
}

void ALimenBaseHUD::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	DestroyWidgets();
	Super::EndPlay(EndPlayReason);
}

void ALimenBaseHUD::ShowHUD()
{
	ShowActiveWidget();
}

void ALimenBaseHUD::HideHUD()
{
	HideActiveWidget();
}

void ALimenBaseHUD::ForceHUDState(const bool bForce)
{
	bForceHideHud = bForce;
}

bool ALimenBaseHUD::GetForcedHudState() const
{
	return bForceHideHud;
}

void ALimenBaseHUD::ShowActiveWidget()
{
	if (!ActiveWidget.IsValid()) { return; }
	
	ShowWidget_Internal(ActiveWidget.Get());
}

void ALimenBaseHUD::HideActiveWidget()
{
	if (!ActiveWidget.IsValid()) { return; }
	
	HideWidget_Internal(ActiveWidget.Get());
}

ULimenWidget* ALimenBaseHUD::GetActiveWidget() const
{
	return ActiveWidget.Get();
}

TArray<TWeakObjectPtr<ULimenWidget>> ALimenBaseHUD::GetWidgetList() const
{
	return WidgetList;
}

void ALimenBaseHUD::DestroyWidgets()
{
}

void ALimenBaseHUD::InitializeWidgets()
{
}

void ALimenBaseHUD::HudInitialized(APlayerController* PlayerController)
{
}

void ALimenBaseHUD::UpdateWidgets(APlayerController* PlayerController, APawn* Pawn)
{
	DestroyWidgets();
	InitializeWidgets();
	
	bAreWidgetsInitialized = true;
	HudInitialized(PlayerController);
}

bool ALimenBaseHUD::CanSwitchWidgetVisibility(UWidget* InWidget) const
{
	return !bForceHideHud;
}

void ALimenBaseHUD::ShowWidget_Internal(ULimenWidget* Widget)
{
	if (!Widget || !CanSwitchWidgetVisibility(Widget) || GetWorld()->bIsTearingDown) return;
	if (!Widget->IsShowing() && Widget->ShowWidget()) ActiveWidget = Widget;
}

void ALimenBaseHUD::HideWidget_Internal(ULimenWidget* Widget)
{
	if (!Widget || !CanSwitchWidgetVisibility(Widget)) return;
	if (Widget->IsShowing() && Widget->HideWidget()) ActiveWidget = nullptr;
}

void ALimenBaseHUD::AddWidgetToList(ULimenWidget* InWidget)
{
	WidgetList.AddUnique(TWeakObjectPtr(InWidget));
}

void ALimenBaseHUD::RemoveWidgetFormList(ULimenWidget* InWidget)
{
	WidgetList.Remove(TWeakObjectPtr(InWidget));
}
