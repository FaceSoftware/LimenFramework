// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenLumenFinalGatherQualitySetting.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


ULimenLumenFinalGatherQualitySetting::ULimenLumenFinalGatherQualitySetting()
{
	DevelopmentName = TEXT("setting_lumenfinalgather");
	Category = FText::FromString(TEXT("Graphics"));
	DisplayName = FText::FromString(TEXT("Lumen Quality"));
	Description = FText::FromString(TEXT("Sets the lumen final gather quality level, higher is better. Increase if seeing flashing light spots in lit areas (aka noise)"));
}

void ULimenLumenFinalGatherQualitySetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(.2f);
	ValueRange.Push(2.f);
	DefaultSettingValue = 2.f;
}

void ULimenLumenFinalGatherQualitySetting::ApplyCurrentSetting()
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

void ULimenLumenFinalGatherQualitySetting::GlobalPostProcessFound(APostProcessVolume* PostProcessVolume)
{
	PostProcessVolume->Settings.bOverride_LumenFinalGatherQuality = true;
	PostProcessVolume->Settings.LumenFinalGatherQuality = GetCurrentValue();
}
