// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenChromaticAberrationSetting.h"

#include "EngineUtils.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenChromaticAberrationSetting::ULimenChromaticAberrationSetting()
{
	DevelopmentName = TEXT("setting_chromaticaberration");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Chromatic Aberration"));
	Description = FText::FromString(TEXT("Distortion effect."));
	DecimalsDisplayed = 2;
}

void ULimenChromaticAberrationSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(1.f);

	DefaultSettingValue = 0.5f;	
	
	if (auto* Subsystem = GetOwnerSubsystem<ULimenGraphicalSettingsSubsystem>())
	{
		Subsystem->OnPostProcessSettingEvaluate.AddUObject(this, &ThisClass::PostProcessSettingEvaluate);
	}
}

void ULimenChromaticAberrationSetting::PostProcessSettingEvaluate(FPostProcessSettings& Settings)
{
	check(GetCurrentValue() >= ValueRange[0] && GetCurrentValue() <= ValueRange[1]);

	Settings.bOverride_SceneFringeIntensity = true;
	Settings.SceneFringeIntensity = GetAppliedValue();
}
