// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenChromaticAberrationSetting.h"

#include "EngineUtils.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenChromaticAberrationSetting::ULimenChromaticAberrationSetting()
{
	bUseRecurrentAction = false;
	DevelopmentName = TEXT("setting_chromaticaberration");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Chromatic Aberration"));
	Description = FText::FromString(TEXT("Distortion effect."));
	MinValuePerChange = .1f;
}

void ULimenChromaticAberrationSetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);

	ULimenGraphicalSettingsSubsystem* GraphicalSettings = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	if (APostProcessVolume* GlobalPostProcess = GraphicalSettings->GetGlobalPostProcess(); GlobalPostProcess != nullptr)
	{
		GlobalPostProcessFound(GlobalPostProcess);
	}
	else
	{
		GraphicalSettings->OnGlobalPostProcessFound.AddUObject(this, &ThisClass::GlobalPostProcessFound);
	}
}

void ULimenChromaticAberrationSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(1.f);

	DefaultSettingValue = 0.5f;
}

void ULimenChromaticAberrationSetting::GlobalPostProcessFound(APostProcessVolume* GlobalPostProcess)
{
	check(GlobalPostProcess->Settings.bOverride_SceneFringeIntensity)
	check(GetCurrentValue() >= ValueRange[0] && GetCurrentValue() <= ValueRange[1]);

	GlobalPostProcess->Settings.SceneFringeIntensity = GetCurrentValue();
}
