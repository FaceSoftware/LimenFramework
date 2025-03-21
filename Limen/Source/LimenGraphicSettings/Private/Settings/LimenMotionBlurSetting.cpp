// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenMotionBlurSetting.h"

#include "EngineUtils.h"
#include "Engine/GameInstance.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenMotionBlurSetting::ULimenMotionBlurSetting()
{
	DevelopmentName = TEXT("setting_motionblur");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Motion Blur"));
	Description = FText::FromString(TEXT("Blurs objects on screen when they move too fast."));
	MinValuePerChange = .1f;
}

void ULimenMotionBlurSetting::ApplyCurrentSetting(bool bUserRequest)
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

void ULimenMotionBlurSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(1.f);

	DefaultSettingValue = 0.5f;
}

void ULimenMotionBlurSetting::GlobalPostProcessFound(APostProcessVolume* GlobalPostProcess)
{
	check(GlobalPostProcess->Settings.bOverride_MotionBlurAmount)
	check(GetCurrentValue() >= ValueRange[0] && GetCurrentValue() <= ValueRange[1]);

	GlobalPostProcess->Settings.MotionBlurAmount = GetCurrentValue();
}
