// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenLumenFinalGatherQualitySetting.generated.h"


class APostProcessVolume;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenLumenFinalGatherQualitySetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenLumenFinalGatherQualitySetting();

protected:
	virtual void SetDefaults() override;
	virtual void ApplyCurrentSetting(bool bUserRequest) override;

private:
	void GlobalPostProcessFound(APostProcessVolume* PostProcessVolume);
	
};
