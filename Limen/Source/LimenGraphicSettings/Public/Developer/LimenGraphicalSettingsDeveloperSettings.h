// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CameraModifiers/LimenGraphicalSettingsCameraModifier.h"
#include "Developer/LimenModularSettingsSubsystemDeveloperSettings.h"
#include "LimenGraphicalSettingsDeveloperSettings.generated.h"


/**
 * 
 */
UCLASS(Config="Limen")
class LIMENGRAPHICSETTINGS_API ULimenGraphicalSettingsDeveloperSettings : public ULimenModularSettingsSubsystemDeveloperSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Config, Category="Graphical Settings")
	TSoftClassPtr<ULimenGraphicalSettingsCameraModifier> CameraModifierClass;
	
	
	ULimenGraphicalSettingsDeveloperSettings()
	{
		SectionName = TEXT("Limen - Graphic Settings");
		CameraModifierClass = ULimenGraphicalSettingsCameraModifier::StaticClass();
	}
};
