// Copyright © FaceSoftware. All rights reserved.


#include "Settings/LimenGlobalIlluminationSetting.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenGlobalIlluminationSetting::ULimenGlobalIlluminationSetting()
{
	DevelopmentName = TEXT("setting_gimode");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Global Illumination"));
	Description = FText::FromString(TEXT(""));
}

void ULimenGlobalIlluminationSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Push(Lumen);
	PossibleSelections.Push(ScreenSpace);
	PossibleSelections.Push(None);

	DefaultSelection = Lumen;
	
	if (auto* Subsystem = GetOwnerSubsystem<ULimenGraphicalSettingsSubsystem>())
	{
		Subsystem->OnPostProcessSettingEvaluate.AddUObject(this, &ThisClass::PostProcessSettingEvaluate);
	}
}

void ULimenGlobalIlluminationSetting::PostProcessSettingEvaluate(FPostProcessSettings& Settings)
{
	const FString AppliedValue = GetAppliedValue();
	check(PossibleSelections.Contains(AppliedValue))

	Settings.bOverride_DynamicGlobalIlluminationMethod = true;
	
	EDynamicGlobalIlluminationMethod::Type Method = EDynamicGlobalIlluminationMethod::None;
	
	if (AppliedValue == Lumen)
	{
		Method = EDynamicGlobalIlluminationMethod::Lumen;
	}
	else if (AppliedValue == ScreenSpace)
	{
		Method = EDynamicGlobalIlluminationMethod::ScreenSpace;
	}
	
	Settings.DynamicGlobalIlluminationMethod = Method;
}
