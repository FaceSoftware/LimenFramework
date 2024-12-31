// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenDisplayMonitorSetting.h"

#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"
#include "Framework/Application/SlateApplication.h"


ULimenDisplayMonitorSetting::ULimenDisplayMonitorSetting()
{
	DevelopmentName = TEXT("setting_displaymonitor");
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("Monitor"));
	Description = FText::FromString(TEXT("Selects which monitor should the game be displayed on."));
}

bool ULimenDisplayMonitorSetting::CanEdit() const
{
	return true;
}

void ULimenDisplayMonitorSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	PossibleSelections.Reserve(DisplayMetrics.MonitorInfo.Num());
	
	for (const FMonitorInfo& Monitor : DisplayMetrics.MonitorInfo)
	{
		if (Monitor.Name != GetCurrentValue())
		{
			continue;
		}

		TSharedPtr<SWindow> GameWindow = FSlateApplication::Get().GetActiveTopLevelWindow();
		if (!GameWindow.IsValid())
		{
			return;
		}
		
		FVector2D Position(Monitor.WorkArea.Left, Monitor.WorkArea.Top);
		GameWindow->MoveWindowTo(Position);
	}
}

void ULimenDisplayMonitorSetting::SetDefaults()
{
	Super::SetDefaults();

	FDisplayMetrics DisplayMetrics;
	FSlateApplication::Get().GetDisplayMetrics(DisplayMetrics);

	PossibleSelections.Reserve(DisplayMetrics.MonitorInfo.Num());
	
	for (const FMonitorInfo& Monitor : DisplayMetrics.MonitorInfo)
	{
		if (Monitor.bIsPrimary)
		{
			DefaultSelection = Monitor.Name;
		}

		PossibleSelections.Push(Monitor.Name);
	}
}
