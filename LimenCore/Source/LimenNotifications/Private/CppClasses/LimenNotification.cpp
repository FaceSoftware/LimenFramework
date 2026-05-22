// Copyright Face Software. All Rights Reserved.


#include "CppClasses/LimenNotification.h"

#include "LogMacros/LimenLogMacros.h"


FLimenNotification::FLimenNotification(const FNotificationParams& InNotificationParams)
{
	Params.DisplayTime = InNotificationParams.DisplayTime;
	Params.NotificationTitle = InNotificationParams.NotificationTitle;
	Params.NotificationMessage = InNotificationParams.NotificationMessage;
}

FLimenNotification::~FLimenNotification()
{
}

bool FLimenNotification::UseCustomDisplayTime() const
{
	return Params.bUseCustomDisplayTime;
}

float FLimenNotification::GetDisplayTime() const
{
	return Params.DisplayTime;
}

const FText& FLimenNotification::GetNotificationTitle() const
{
	return Params.NotificationTitle;
}

const FText& FLimenNotification::GetNotificationMessage() const
{
	return Params.NotificationMessage;
}

bool FLimenNotification::operator==(const FLimenNotification* Other) const
{
	return Params.NotificationTitle.ToString() == Other->Params.NotificationTitle.ToString() &&
		Params.NotificationMessage.ToString() == Other->Params.NotificationMessage.ToString();
}

FLimenNotification::operator FNotificationParams() const
{
	return GetParameters();
}

const FNotificationParams& FLimenNotification::GetParameters() const
{
	return Params;
}
