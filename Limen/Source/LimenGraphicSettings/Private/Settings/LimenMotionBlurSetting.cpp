// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenMotionBlurSetting.h"

#include "Engine/GameInstance.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenMotionBlurSetting::ULimenMotionBlurSetting()
{
	DevelopmentName = TEXT("setting_motionblur");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Motion Blur"));
	Description = FText::FromString(TEXT("Blurs objects on screen when they move too fast."));
	DecimalsDisplayed = 2;
}

void ULimenMotionBlurSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(1.f);

	DefaultSettingValue = 0.5f;
	
	ULimenGraphicalSettingsSubsystem* GraphicalSettings = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	GraphicalSettings->OnPostProcessSettingEvaluate.AddUObject(this, &ThisClass::PostProcessSettingsEvaluate);
}

void ULimenMotionBlurSetting::PostProcessSettingsEvaluate(FPostProcessSettings& Settings)
{
	const float Value = GetAppliedValue();
	check(Value >= ValueRange[0] && Value <= ValueRange[1]);

	Settings.bOverride_MotionBlurAmount = true;
	Settings.MotionBlurAmount = Value;
}
