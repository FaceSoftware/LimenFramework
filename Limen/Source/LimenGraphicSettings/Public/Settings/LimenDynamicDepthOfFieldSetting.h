// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenDynamicDepthOfFieldSetting.generated.h"

class APostProcessVolume;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenDynamicDepthOfFieldSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenDynamicDepthOfFieldSetting();
	
protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

	virtual void GlobalPostProcessFound(APostProcessVolume* PostProcess);
	
private:
	inline static const FString Enabled		= TEXT("Enabled");
	inline static const FString Disabled	= TEXT("Disabled");
};
