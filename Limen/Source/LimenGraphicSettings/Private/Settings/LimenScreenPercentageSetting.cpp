// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenScreenPercentageSetting.h"


ULimenScreenPercentageSetting::ULimenScreenPercentageSetting()
{
	DevelopmentName = TEXT("setting_screenpercentage");
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("Screen Percentage"));
	Description = FText::FromString(TEXT(""));
	DecimalsDisplayed = 0;
}

void ULimenScreenPercentageSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();
	
	Setting.ResetCVars();
	Setting.AddCVar("r.ScreenPercentage", GetCurrentValue());
	Setting.ApplyCVars();
}

void ULimenScreenPercentageSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(50.f);
	ValueRange.Push(100.f);
	DefaultSettingValue = 90.f;
}
