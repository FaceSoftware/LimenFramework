// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenChromaticAberrationSetting.generated.h"


class APostProcessVolume;

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenChromaticAberrationSetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenChromaticAberrationSetting();

protected:
	virtual void SetDefaults() override;

private:	
	void PostProcessSettingEvaluate(FPostProcessSettings& Settings);
};
