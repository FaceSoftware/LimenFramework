// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenNotificationComponent.generated.h"


class ULimenNotificationWidget;
class FLimenNotification;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENNOTIFICATIONS_API ULimenNotificationComponent : public UActorComponent
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotificationPauseRequestDelegate, const bool, bWantsToPause);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNotificationDisplayedDelegate, const bool, bWasConsetNotification);
	
public:
	FNotificationPauseRequestDelegate OnPauseRequested;
	FNotificationDisplayedDelegate OnNotificationDisplayed;

	explicit ULimenNotificationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	void QueueNotification(const TSharedRef<FLimenNotification>& InNotification);
	void SetNotificationsHidden(const bool bHide);

protected:
	void TryAssignPlayerController();

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	bool bIgnoreDuplicate;
	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	bool bConsentPauseGame;	
	
	UPROPERTY(EditAnywhere, Category="Limen|Classes")
	TSoftClassPtr<ULimenNotificationWidget> NotificationWidgetClass;
	UPROPERTY()
	TObjectPtr<ULimenNotificationWidget> NotificationWidget;
	TArray<TSharedRef<FLimenNotification>> PendingNotifications;
	FTimerHandle NotificationTimer;
	
	bool bShowHints;
	
	UPROPERTY()
	TObjectPtr<APlayerController> BoundPlayerController;

	void DisplayNextNotification();
	void DestroyLastNotification();

	bool IsNotificationDuplicate(const TSharedRef<FLimenNotification>& Test);
};
