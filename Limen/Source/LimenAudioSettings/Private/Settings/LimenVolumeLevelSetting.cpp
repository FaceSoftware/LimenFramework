// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenVolumeLevelSetting.h"

#include "Sound/SoundClass.h"


ULimenVolumeLevelSetting::ULimenVolumeLevelSetting()
{
	DevelopmentName = FName(TEXT("setting_"));
	DisplayName = FText();
	Category = FText();
}

void ULimenVolumeLevelSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	check(!SoundClass.IsNull())
	SoundClass.LoadSynchronous()->Properties.Volume = GetCurrentValue();
}

void ULimenVolumeLevelSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(1.f);
	DefaultSettingValue = 1.f;
}
