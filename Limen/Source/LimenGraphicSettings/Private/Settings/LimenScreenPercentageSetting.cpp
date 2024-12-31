// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenScreenPercentageSetting.h"

#include "HAL/IConsoleManager.h"


ULimenScreenPercentageSetting::ULimenScreenPercentageSetting()
{
	DevelopmentName = TEXT("setting_screenpercentage");
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("Screen Percentage"));
	Description = FText::FromString(TEXT(""));
}

void ULimenScreenPercentageSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	IConsoleVariable* CVarScreenPercentage = IConsoleManager::Get().FindConsoleVariable(TEXT("r.ScreenPercentage"));
	CVarScreenPercentage->Set(GetCurrentValue(), EConsoleVariableFlags::ECVF_SetByGameOverride);
}

void ULimenScreenPercentageSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(50.f);
	ValueRange.Push(100.f);
	DefaultSettingValue = 100.f;
}
