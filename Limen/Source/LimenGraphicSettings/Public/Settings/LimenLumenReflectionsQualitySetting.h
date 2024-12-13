// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenLumenReflectionsQualitySetting.generated.h"

class ULimenGraphicalSettingsSubsystem;
class APostProcessVolume;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenLumenReflectionsQualitySetting : public ULimenValueSetting
{
	GENERATED_BODY()
	
public:
	ULimenLumenReflectionsQualitySetting();

protected:
	virtual void SetDefaults() override;
	virtual void ApplyCurrentSetting() override;

private:
	void GlobalPostProcessFound(APostProcessVolume* PostProcessVolume);
};
