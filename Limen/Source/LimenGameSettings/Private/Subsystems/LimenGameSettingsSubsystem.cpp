// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenGameSettingsSubsystem.h"

#include "Developer/LimenGameSettingsDeveloperSettings.h"


ULimenGameSettingsSubsystem::ULimenGameSettingsSubsystem()
{
	SaveDataName = TEXT("GameSettings");
}

bool ULimenGameSettingsSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void ULimenGameSettingsSubsystem::LoadDefaultSettingsList()
{
	Super::LoadDefaultSettingsList();

	SubsystemSettings = GetDefault<ULimenGameSettingsDeveloperSettings>();
	for (const auto& SettingClass : SubsystemSettings->SettingsList)
	{
		if (SettingClass.IsNull())
		{
			// Don't crash the editor if the class is not set
			continue;
		}
		
		ULimenSetting* NewSetting = NewObject<ULimenSetting>(this, SettingClass.LoadSynchronous());
		check(NewSetting != nullptr);
		NewSetting->InitializeSetting(this);
		AddItem(NewSetting);
	}
}
