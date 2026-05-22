// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenNotificationWidget.h"

#include "Animation/WidgetAnimation.h"


ULimenNotificationWidget::ULimenNotificationWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetWidgetLevel(9);
}

void ULimenNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetDefaultVisibleState(ESlateVisibility::HitTestInvisible);
}

void ULimenNotificationWidget::SetNotificationParams(const float InDisplayTime, const FText& InNotificationTitle,
                                                     const FText& InNotificationMessage)
{
	DisplayTime = InDisplayTime;
	NotificationTitle = InNotificationTitle;
	NotificationMessage = InNotificationMessage;
	
	NotificationParamsUpdated(DisplayTime, NotificationTitle, NotificationMessage);
}

float ULimenNotificationWidget::GetDisplayTime() const
{
	return DisplayTime;
}

FText& ULimenNotificationWidget::GetNotificationTitle()
{
	return NotificationTitle;
}

FText& ULimenNotificationWidget::GetNotificationMessage()
{
	return NotificationMessage;
}

double ULimenNotificationWidget::GetDelayTime(UWidgetAnimation* InShowAnimation, UWidgetAnimation* InHideAnimation, bool& bIsPositive) const
{
	const double DelayTime = DisplayTime - (ShowAnimation->GetEndTime() + HideAnimation->GetEndTime());
	bIsPositive = !FMath::IsNegativeOrNegativeZero(DelayTime);
	return DelayTime;
}
