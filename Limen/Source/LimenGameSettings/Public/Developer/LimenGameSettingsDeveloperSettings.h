// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Developer/LimenModularSettingsSubsystemDeveloperSettings.h"
#include "LimenGameSettingsDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config="Limen")
class LIMENGAMESETTINGS_API ULimenGameSettingsDeveloperSettings : public ULimenModularSettingsSubsystemDeveloperSettings
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, Config, Category="Game Settings")
	bool bUseSubsystem = false;
	UPROPERTY(EditAnywhere, Config, Category="Game Settings")
	TArray<TSoftClassPtr<ULimenSetting>> SettingsList;

	ULimenGameSettingsDeveloperSettings()
	{
		SectionName = TEXT("Limen - Game Settings");
	}
};
