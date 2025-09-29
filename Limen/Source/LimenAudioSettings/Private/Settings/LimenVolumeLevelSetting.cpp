// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenVolumeLevelSetting.h"

#include "FMODBlueprintStatics.h"
#include "Engine/GameInstance.h"
#include "LogMacros/LimenLogMacros.h"
#include "Sound/SoundClass.h"


ULimenVolumeLevelSetting::ULimenVolumeLevelSetting()
{
	DevelopmentName = FName(TEXT("setting_"));
	DisplayName = FText();
	Category = FText();
	DecimalsDisplayed = 2;
	FmodBus = nullptr;
}

void ULimenVolumeLevelSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	if (FmodBus) FmodBus->setVolume(GetCurrentValue());
	if (SoundClass) SoundClass->Properties.Volume = GetCurrentValue();
}

void ULimenVolumeLevelSetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0.f);
	ValueRange.Push(1.f);
	DefaultSettingValue = 1.f;

	const FMOD::Studio::System* StudioSystem = IFMODStudioModule::Get().GetStudioSystem(EFMODSystemContext::Runtime);
	const FString BusPath = TEXT("bus:/") + FmodBusName;
	const FMOD_RESULT Result = StudioSystem->getBus(TCHAR_TO_UTF8(*BusPath), &FmodBus);
	if (Result == FMOD_OK) { return; }

	LIMEN_LOG(LogTemp, Warning, this, TEXT("Failed to get Bus! Error: %d"), Result);
}
