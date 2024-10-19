// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenAudioSettingsSubsystem.h"

#include "Developer/LimenAudioDeveloperSettings.h"


ULimenAudioSettingsSubsystem::ULimenAudioSettingsSubsystem()
{
	SaveDataName = TEXT("AudioSettings");
}

void ULimenAudioSettingsSubsystem::LoadDefaultSettingsList()
{
	Super::LoadDefaultSettingsList();

	SubsystemSettings = GetDefault<ULimenAudioDeveloperSettings>();
	for (const auto& SettingClass : SubsystemSettings->SettingsList)
	{
		if (SettingClass.IsNull())
		{
			// Don't crash the editor if the class is not set
			continue;
		}
		
		ULimenSetting* NewSetting = NewObject<ULimenSetting>(this, SettingClass.LoadSynchronous());
		check(NewSetting != nullptr);
		NewSetting->InitializeSetting();
		AddItem(NewSetting);
	}
}
