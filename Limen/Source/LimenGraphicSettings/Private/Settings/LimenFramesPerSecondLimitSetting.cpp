// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenFramesPerSecondLimitSetting.h"

#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"


ULimenFramesPerSecondLimitSetting::ULimenFramesPerSecondLimitSetting()
{
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("FPS Limit"));
	Description = FText::FromString(TEXT("Limits the amount of frames displayed each second. Zero is unlimited."));
}

void ULimenFramesPerSecondLimitSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	float Value = GetCurrentValue();
	if (Value < 30.f && Value > 0)
	{
		Value = 30.f;
		SetNewValue(Value);
	}

	GEngine->GetGameUserSettings()->SetFrameRateLimit(Value);
	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void ULimenFramesPerSecondLimitSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(360.f);

	DefaultSettingValue = 0.f;
}
