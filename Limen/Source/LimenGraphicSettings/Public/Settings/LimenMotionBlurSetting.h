// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenValueSetting.h"
#include "LimenMotionBlurSetting.generated.h"

class APostProcessVolume;


/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenMotionBlurSetting : public ULimenValueSetting
{
	GENERATED_BODY()

public:
	ULimenMotionBlurSetting();

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

private:	
	void GlobalPostProcessFound(APostProcessVolume* GlobalPostProcess);
};
