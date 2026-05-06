// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenGlobalIlluminationSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenGlobalIlluminationSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	inline static const FString None = TEXT("Disabled");
	inline static const FString ScreenSpace = TEXT("Screen Space");
	inline static const FString Lumen = TEXT("Lumen");
	
	ULimenGlobalIlluminationSetting();
	
protected:
	virtual void SetDefaults() override;
	
private:	
	void PostProcessSettingEvaluate(FPostProcessSettings& Settings);
};
