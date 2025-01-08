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
	ULimenGameSettingsDeveloperSettings()
	{
		SectionName = TEXT("Limen - Game Settings");
	}
};
