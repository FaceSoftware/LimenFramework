// Copyright Face Software. All Rights Reserved.


#include "CppClasses/LimenNotification.h"

#include "LogMacros/LimenLogMacros.h"


FLimenNotification::FLimenNotification(const FNotificationParams& InNotificationParams)
{
	DisplayTime = InNotificationParams.DisplayTime;
	NotificationTitle = InNotificationParams.NotificationTitle;
	NotificationMessage = InNotificationParams.NotificationMessage;
}

FLimenNotification::~FLimenNotification()
{
}

float FLimenNotification::GetDisplayTime() const
{
	return DisplayTime;
}

const FText& FLimenNotification::GetNotificationTitle()
{
	return NotificationTitle;
}

const FText& FLimenNotification::GetNotificationMessage()
{
	return NotificationMessage;
}

bool FLimenNotification::operator==(const FLimenNotification* Other) const
{
	return NotificationTitle.ToString() == Other->NotificationTitle.ToString() && NotificationMessage.ToString() == Other->NotificationMessage.ToString();
}
