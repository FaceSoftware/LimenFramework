// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenWidget.h"
#include "LimenNotificationWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class LIMENNOTIFICATIONS_API ULimenNotificationWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	explicit ULimenNotificationWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	
	void SetNotificationParams(const float InDisplayTime, const FText& InNotificationTitle,
	                           const FText& InNotificationMessage);

protected:	
	UPROPERTY(BlueprintReadWrite, Category="Limen|Gameplay|Notifications")
	TObjectPtr<UWidgetAnimation> WidgetAnimation;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay|Notifications")
	float GetDisplayTime() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay|Notifications", BlueprintPure)
	FText& GetNotificationTitle();
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay|Notifications", BlueprintPure)
	FText& GetNotificationMessage();

	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay|Notifications|Helpers", BlueprintPure)
	double GetDelayTime(UWidgetAnimation* ShowAnimation, UWidgetAnimation* HideAnimation, bool& bIsPositive);

	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Gameplay|Notifications")
	void NotificationParamsUpdated(const float InDisplayTime, const FText& InNotificationTitle, const FText& InNotificationMessage);
	
private:
	UPROPERTY(BlueprintGetter=GetDisplayTime, Category="Limen|Notifications")
	float DisplayTime;
	UPROPERTY(BlueprintGetter=GetNotificationTitle, Category="Limen|Notifications")
	FText NotificationTitle;
	UPROPERTY(BlueprintGetter=GetNotificationMessage, Category="Limen|Notifications")
	FText NotificationMessage;

	FTimerHandle NotificationTimer;
};
