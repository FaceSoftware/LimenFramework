// Copyright Face Software. All Rights Reserved.


#include "Components/LimenNotificationComponent.h"

#include "Blueprint/UserWidget.h"
#include "LogMacros/LimenLogMacros.h"
#include "Widgets/LimenNotificationWidget.h"
#include "CppClasses/LimenNotification.h"
#include "GameFramework/HUD.h"


ULimenNotificationComponent::ULimenNotificationComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;
	bIgnoreDuplicate = true;
	bConsentPauseGame = true;
	bShowHints = true;
}

void ULimenNotificationComponent::QueueNotification(const TSharedRef<FLimenNotification>& InNotification)
{
	TryAssignPlayerController();
	
	if (bIgnoreDuplicate && IsNotificationDuplicate(InNotification))
	{
		LIMEN_LOG(LogLimenCore, Warning, this, "Ignoring duplicated notification. In queue: %d", PendingNotifications.Num())
		return;
	}
	
	PendingNotifications.Push(InNotification);
	LIMEN_LOG(LogLimenCore, Log, this, "Queued new notification. Total queued (including this one): %d", PendingNotifications.Num())

	if (PendingNotifications.Num() == 1)
	{
		DisplayNextNotification();
	}
}

void ULimenNotificationComponent::SetNotificationsHidden(const bool bHide)
{
	if (bHide)
	{
		if (NotificationWidget)
		{
			NotificationWidget->HideWidget();
		}
	}
	else
	{
		if (!PendingNotifications.IsEmpty() && NotificationWidget)
		{
			NotificationWidget->ShowWidget();
		}
	}
}

void ULimenNotificationComponent::TryAssignPlayerController()
{
	const AHUD* OwnerHud = Cast<AHUD>(GetOwner());
	checkf(OwnerHud != nullptr, TEXT("Owner must be a AHUD derived class"));
	BoundPlayerController = OwnerHud->GetOwningPlayerController();

	if (!BoundPlayerController)
	{
		LIMEN_LOG(LogLimenCore, Error, this,
		  "Attempted to get player controller but could not find one. Notifications won't be displayed.");
	}
}

void ULimenNotificationComponent::DisplayNextNotification()
{
	if (PendingNotifications.IsEmpty())
	{
		NotificationWidget->HideWidget();
		NotificationTimer.Invalidate();
		GetWorld()->GetTimerManager().ClearTimer(NotificationTimer);
		return;
	}

	if (!IsValid(NotificationWidget.Get()))
	{
		if (NotificationWidgetClass.IsNull())
		{
			LIMEN_LOG(LogLimenCore, Error, this, "Notification widget class is invalid!")
			return;
		}

		NotificationWidget = CreateWidget<ULimenNotificationWidget>(BoundPlayerController, NotificationWidgetClass.LoadSynchronous());
		NotificationWidget->HideWidget();
	}

	const auto Notification = PendingNotifications[0];
	NotificationWidget->SetNotificationParams(Notification->GetDisplayTime(), Notification->GetNotificationTitle(),
	                                          Notification->GetNotificationMessage());

	NotificationWidget->ShowWidget();
	GetWorld()->GetTimerManager().SetTimer(NotificationTimer, this, &ThisClass::DestroyLastNotification,
	                                       Notification->GetDisplayTime(), false);
}

void ULimenNotificationComponent::DestroyLastNotification()
{
	const TSharedPtr<FLimenWidgetHidden> OnWidgetHidden = MakeShared<FLimenWidgetHidden>();
	OnWidgetHidden->BindLambda([this]
	{
		LIMEN_LOG(LogLimenCore, Log, this, "Removing last displayed notification. Reference count (before removal): %d",
		PendingNotifications[0].GetSharedReferenceCount());
		PendingNotifications.RemoveAt(0);
		DisplayNextNotification();
	});
	
	NotificationWidget->HideWidget(OnWidgetHidden);
}

bool ULimenNotificationComponent::IsNotificationDuplicate(const TSharedRef<FLimenNotification>& Test)
{
	const TSharedPtr<const FLimenNotification> TestPtr = Test.ToSharedPtr();
	
	for (const auto Notification : PendingNotifications)
	{
		if (Test.Get() == Notification.ToSharedPtr().Get())
		{
			return true;
		}
	}

	return false;
}
