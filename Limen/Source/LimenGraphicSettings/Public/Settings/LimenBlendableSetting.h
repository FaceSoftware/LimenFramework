// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenBlendableSetting.generated.h"

class APostProcessVolume;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenBlendableSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");
	
	ULimenBlendableSetting();

protected:
	UPROPERTY(EditAnywhere, Category = "Blendable Setting")
	int32 BlendableIndex;
	
	virtual void SetDefaults() override;
	
private:
	void PostProcessSettingEvaluate(FPostProcessSettings& Settings);
};
