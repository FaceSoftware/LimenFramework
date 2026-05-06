// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenBrightnessSetting.h"

#include "TimerManager.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenBrightnessSetting::ULimenBrightnessSetting()
{
	DevelopmentName = TEXT("setting_antialiasing");

	const FString CategoryStr = TEXT("Display");
	const FString DisplayNameStr = TEXT("Brightness");
	const FString DescriptionStr = TEXT("Adjust display brightness");

	Category = FText::FromString(CategoryStr);
	DisplayName = FText::FromString(DisplayNameStr);
	Description = FText::FromString(DescriptionStr);
}

const TArray<FBrightnessSettingValues>& ULimenBrightnessSetting::GetSettingValues() const
{
	return DummyData;
}

FBrightnessSettingValues ULimenBrightnessSetting::GetCurrentValue() const
{
	return CurrentSelection;
}

FBrightnessSettingValues ULimenBrightnessSetting::GetPreviousValue() const
{
	return PreviousSelection;
}

FBrightnessSettingValues ULimenBrightnessSetting::GetAppliedValue() const
{
	return AppliedSelection;
}

bool ULimenBrightnessSetting::IsValueValid(const FBrightnessSettingValues& Test)
{
	return FMath::IsFinite(Test.HighToneValue) && FMath::IsFinite(Test.LowToneValue);
}

bool ULimenBrightnessSetting::CanEdit() const
{
	return true;
}

bool ULimenBrightnessSetting::SetNewValue(const FBrightnessSettingValues& NewSelection)
{
	if (!TLimenEditableSetting<FBrightnessSettingValues>::SetNewValue(NewSelection))
	{
		return false;
	}

	PreviousSelection = CurrentSelection;
	CurrentSelection = NewSelection;
	OnSettingUpdated.Broadcast(this);
	return true;
}

void ULimenBrightnessSetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);
}

void ULimenBrightnessSetting::SetDefaults()
{
	Super::SetDefaults();

	DefaultSelection = FBrightnessSettingValues();
	
	if (auto* Subsystem = GetOwnerSubsystem<ULimenGraphicalSettingsSubsystem>())
	{
		Subsystem->OnPostProcessSettingEvaluate.AddUObject(this, &ThisClass::PostProcessSettingEvaluate);
	}
}

void ULimenBrightnessSetting::SetDefaultValue()
{
	Super::SetDefaultValue();

	CurrentSelection = DefaultSelection;
}

void ULimenBrightnessSetting::PostDataLoaded()
{
	PreviousSelection = CurrentSelection = AppliedSelection;
	Super::PostDataLoaded();
}

void ULimenBrightnessSetting::PostProcessSettingEvaluate(FPostProcessSettings& Settings) const
{
	Settings.bOverride_ColorGainHighlights = true;
	Settings.ColorGainHighlights.W = AppliedSelection.HighToneValue;
	Settings.bOverride_ColorGainShadows = true;
	Settings.ColorGainShadows.W = AppliedSelection.LowToneValue;

	OnSettingApplied.Broadcast(this);
}
