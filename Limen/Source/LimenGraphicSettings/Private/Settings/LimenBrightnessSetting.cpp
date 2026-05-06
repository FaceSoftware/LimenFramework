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

	auto* GraphicsSubsystem = GetOwnerSubsystem<ULimenGraphicalSettingsSubsystem>();
	if (APostProcessVolume* GlobalPostProcess = GraphicsSubsystem->GetGlobalPostProcess(); GlobalPostProcess == nullptr)
	{
		GraphicsSubsystem->OnGlobalPostProcessFound.AddUObject(this, &ThisClass::GlobalPostProcessFound);
	}
	else
	{
		GlobalPostProcessFound(GlobalPostProcess);
	}
}

void ULimenBrightnessSetting::SetDefaults()
{
	Super::SetDefaults();

	DefaultSelection = FBrightnessSettingValues();
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

void ULimenBrightnessSetting::GlobalPostProcessFound(APostProcessVolume* InGlobalPostProcess) const
{
	InGlobalPostProcess->Settings.bOverride_ColorGainHighlights = true;
	InGlobalPostProcess->Settings.ColorGainHighlights.W = CurrentSelection.HighToneValue;

	InGlobalPostProcess->Settings.bOverride_ColorGainShadows = true;
	InGlobalPostProcess->Settings.ColorGainShadows.W = CurrentSelection.LowToneValue;

	OnSettingApplied.Broadcast(this);
}
