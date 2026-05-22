// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Developer/LimenModularSettingsSubsystemDeveloperSettings.h"
#include "LimenAudioDeveloperSettings.generated.h"

/**
 * 
 */
UCLASS(Config="Limen")
class LIMENAUDIOSETTINGS_API ULimenAudioDeveloperSettings : public ULimenModularSettingsSubsystemDeveloperSettings
{
	GENERATED_BODY()

public:
	ULimenAudioDeveloperSettings()
	{
		SectionName = TEXT("Limen - Audio Settings");
	}
};
