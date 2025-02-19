// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenNotificationComponent.generated.h"


struct FNotificationParams;
class FLimenNotification;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENNOTIFICATIONS_API ULimenNotificationComponent : public UActorComponent
{
	GENERATED_BODY()
	
	
public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNewNotificationDelegate, const FNotificationParams&, NotificationParams);

	UPROPERTY(BlueprintAssignable)
	FNewNotificationDelegate OnNewNotification;

	explicit ULimenNotificationComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void QueueNotification(const TSharedRef<const FLimenNotification>& InNotification);

protected:
	void TryAssignPlayerController();

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	float DefaultNotificationDisplayTime;
	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	bool bIgnoreDuplicate;

	UPROPERTY(EditDefaultsOnly, Category="Limen|Rules")
	int32 MaxConsecutiveNotifications;
	
	TArray<TSharedRef<const FLimenNotification>> PendingNotifications;
	TMap<TSharedRef<const FLimenNotification>, FTimerHandle> ActiveNotifications;
	
	UPROPERTY()
	TObjectPtr<APlayerController> BoundPlayerController;

	void DisplayNextNotification();
	void NotificationDisplayTimeReached(const TSharedRef<const FLimenNotification> Notification);
	
	bool IsNotificationDuplicate(const TSharedRef<const FLimenNotification>& Test);
};
