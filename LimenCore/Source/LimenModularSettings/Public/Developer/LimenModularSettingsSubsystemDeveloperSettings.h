// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "LimenModularSettingsSubsystemDeveloperSettings.generated.h"


class ULimenSetting;

/**
 * Base class for developer settings for a modular settings subsystem.
 * Used to add settings to a specific subsystem
 */
UCLASS(Config="Limen", Abstract)
class LIMENMODULARSETTINGS_API ULimenModularSettingsSubsystemDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	ULimenModularSettingsSubsystemDeveloperSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("Modular Settings");	
	}

	virtual FName GetContainerName() const override
	{
		return TEXT("Project");
	}

#if WITH_EDITOR

	virtual FText GetSectionText() const override
	{
		return FText::FromName(SectionName);
	}
	
#endif
};
