// Copyright Face Software. All Rights Reserved.


#include "Components/LimenNotificationComponent.h"

#include "LogMacros/LimenLogMacros.h"
#include "CppClasses/LimenNotification.h"
#include "GameFramework/HUD.h"


ULimenNotificationComponent::ULimenNotificationComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryComponentTick.bCanEverTick = false;

	DefaultNotificationDisplayTime = 3.f;
	bIgnoreDuplicate = true;
	MaxConsecutiveNotifications = 6;

	PendingNotifications.Reserve(MaxConsecutiveNotifications);
	ActiveNotifications.Reserve(MaxConsecutiveNotifications);
}

void ULimenNotificationComponent::QueueNotification(const TSharedRef<const FLimenNotification>& InNotification)
{
	TryAssignPlayerController();
	
	if (bIgnoreDuplicate && IsNotificationDuplicate(InNotification))
	{
		LIMEN_LOG(LogLimenCore, Warning, this, "Ignoring duplicated notification. In queue: %d", PendingNotifications.Num())
		return;
	}

	PendingNotifications.Push(InNotification);
	LIMEN_LOG(LogLimenCore, Log, this, "Queued new notification. Total queued (including this one): %d", PendingNotifications.Num())

	if (ActiveNotifications.Num() < MaxConsecutiveNotifications)
	{
		DisplayNextNotification();
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
		return;
	}
	
	const TSharedRef<const FLimenNotification>& Notification = PendingNotifications.Pop(EAllowShrinking::Yes);
	ActiveNotifications.Add(Notification, FTimerHandle());	

	const float DisplayTime = Notification->UseCustomDisplayTime() ?
		Notification->GetDisplayTime() : DefaultNotificationDisplayTime;

	GetWorld()->GetTimerManager().SetTimer(ActiveNotifications[Notification], [this, Notification]
	{
		ActiveNotifications.Remove(Notification);
		ActiveNotifications.CompactStable();
		DisplayNextNotification();
	}
	, DisplayTime, false);
	
	OnNewNotification.Broadcast(Notification->GetParameters());
}

bool ULimenNotificationComponent::IsNotificationDuplicate(const TSharedRef<const FLimenNotification>& Test)
{
	const TSharedPtr<const FLimenNotification> TestPtr = Test.ToSharedPtr();
	
	for (const auto Notification : PendingNotifications)
	{
		if (Test.Get() == Notification.ToSharedPtr().Get())
		{
			return true;
		}
	}
	for (const TTuple<TSharedRef<const FLimenNotification>, FTimerHandle> Notification : ActiveNotifications)
	{
		if (Test.Get() == Notification.Key.ToSharedPtr().Get())
		{
			return true;
		}
	}

	return false;
}
