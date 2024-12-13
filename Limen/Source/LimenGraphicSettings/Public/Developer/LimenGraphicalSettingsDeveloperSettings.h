// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Developer/LimenModularSettingsSubsystemDeveloperSettings.h"
#include "LimenGraphicalSettingsDeveloperSettings.generated.h"

class ULimenSetting;
/**
 * 
 */
UCLASS(Config="Limen")
class LIMENGRAPHICSETTINGS_API ULimenGraphicalSettingsDeveloperSettings : public ULimenModularSettingsSubsystemDeveloperSettings
{
	GENERATED_BODY()

public:	
	UPROPERTY(EditAnywhere, Config, Category="Graphical Settings")
	FName GlobalPostProcessTag = TEXT("GlobalPostProcess");
	UPROPERTY(EditAnywhere, Config, Category="Graphical Settings")
	bool bUseSubsystem = false;
	UPROPERTY(EditAnywhere, Config, Category="Graphical Settings")
	TArray<TSoftClassPtr<ULimenSetting>> SettingsList;

	ULimenGraphicalSettingsDeveloperSettings()
	{
		SectionName = TEXT("Limen - Graphic Settings");
	}
};
