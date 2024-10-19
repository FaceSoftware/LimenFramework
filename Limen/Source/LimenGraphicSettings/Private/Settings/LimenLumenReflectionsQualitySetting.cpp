// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenLumenReflectionsQualitySetting.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenLumenReflectionsQualitySetting::ULimenLumenReflectionsQualitySetting()
{
	DevelopmentName = TEXT("setting_lumenfinalgather");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Lumen Reflections Quality"));
	Description = FText::FromString(TEXT("Sets the lumen reflections quality level, higher is better."));
}

void ULimenLumenReflectionsQualitySetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(.2f);
	ValueRange.Push(2);
	DefaultSettingValue = 2.f;
}

void ULimenLumenReflectionsQualitySetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	auto* GraphicalSettings = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	if (auto* GlobalPostProcess = GraphicalSettings->GetGlobalPostProcess(); GlobalPostProcess != nullptr)
	{
		GlobalPostProcessFound(GlobalPostProcess);
	}
	else
	{
		GraphicalSettings->OnGlobalPostProcessFound.AddUObject(this, &ThisClass::GlobalPostProcessFound);
	}
}

void ULimenLumenReflectionsQualitySetting::GlobalPostProcessFound(APostProcessVolume* PostProcessVolume)
{
	PostProcessVolume->Settings.bOverride_LumenReflectionQuality = true;
	PostProcessVolume->Settings.LumenReflectionQuality = GetCurrentValue();
}
