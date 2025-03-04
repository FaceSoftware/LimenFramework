// Copyright Face Software. All Rights Reserved.


#include "HUD/LimenBaseHUD.h"

#include "Components/LimenNotificationComponent.h"
#include "CppClasses/LimenNotification.h"
#include "Widgets/LimenWidget.h"


ALimenBaseHUD::ALimenBaseHUD() : Super()
{
	PrimaryActorTick.bTickEvenWhenPaused = true;
	
	NotificationComponent = CreateDefaultSubobject<ULimenNotificationComponent>(TEXT("NotificationComponent"));
	bAreWidgetsInitialized = false;
	SetTickableWhenPaused(true);
	bForceHideHud = 0;

#if WITH_EDITORONLY_DATA

	bDisableLoadingScreenInPIE = false;

#endif
	
}

void ALimenBaseHUD::ShowHUD()
{
	Super::ShowHUD();

	bShowHUD ? ShowActiveWidget() : HideActiveWidget();
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
	if (!ActiveWidget)
	{
		return;
	}
	
	ShowWidget_Internal(ActiveWidget.Get());
}

void ALimenBaseHUD::HideActiveWidget()
{
	if (!ActiveWidget)
	{
		return;
	}
	
	HideWidget_Internal(ActiveWidget.Get());
}

ULimenNotificationComponent* ALimenBaseHUD::GetNotificationsComponent() const
{
	return NotificationComponent.Get();
}

ULimenWidget* ALimenBaseHUD::GetActiveWidget() const
{
	return ActiveWidget.Get();
}

void ALimenBaseHUD::DestroyWidgets()
{
}

void ALimenBaseHUD::InitializeWidgets()
{
}

void ALimenBaseHUD::QueueNotification(const FNotificationParams& InParams)
{	
	NotificationComponent->QueueNotification(MakeShared<FLimenNotification>(InParams));
}

void ALimenBaseHUD::UpdateWidgets(APlayerController* PlayerController, APawn* Pawn)
{
	DestroyWidgets();
	InitializeWidgets();
	
	bAreWidgetsInitialized = true;
	HudInitialized(PlayerController); // C++ call (priority)
	BP_HudInitialized(PlayerController); // Blueprint events call
	OnHudInitialized.Broadcast(this);
}

bool ALimenBaseHUD::CanSwitchWidgetVisibility(UWidget* InWidget) const
{
	return !bForceHideHud;
}

void ALimenBaseHUD::ShowWidget_Internal(ULimenWidget* Widget)
{
	check(Widget != nullptr)
	
	if (!CanSwitchWidgetVisibility(Widget))
	{
		return;
	}

	if (!Widget->IsShowing())
	{
		Widget->ShowWidget();	
	}

	ActiveWidget = Widget;
}

void ALimenBaseHUD::HideWidget_Internal(ULimenWidget* Widget)
{
	check(Widget != nullptr)
	
	if (!CanSwitchWidgetVisibility(Widget))
	{
		return;
	}

	ActiveWidget = nullptr;
	if (Widget->IsShowing())
	{
		Widget->HideWidget();
	}
}
