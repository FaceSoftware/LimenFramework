// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenVolumeLevelSetting.h"

#include "FMODBlueprintStatics.h"
#include "Sound/SoundClass.h"


ULimenVolumeLevelSetting::ULimenVolumeLevelSetting()
{
	DevelopmentName = FName(TEXT("setting_"));
	DisplayName = FText();
	Category = FText();
	DecimalsDisplayed = 2;
}

void ULimenVolumeLevelSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	check(!SoundClass.IsNull())
	SoundClass.LoadSynchronous()->Properties.Volume = GetCurrentValue();
	UFMODBlueprintStatics::SetGlobalParameterByName(FMODParameterName, GetCurrentValue());
}

void ULimenVolumeLevelSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(1.f);
	DefaultSettingValue = 1.f;
}
