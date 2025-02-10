// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenCameraDistortionSetting.generated.h"

class APostProcessVolume;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenCameraDistortionSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");

	UFUNCTION(BlueprintCallable, meta=(WorldContext=WorldContext))
	static void SetCameraDistortionState(UObject* WorldContext, const bool bEnable);
	UFUNCTION(BlueprintCallable, BlueprintPure, meta=(WorldContext=WorldContext))
	static bool GetCameraDistortionState(UObject* WorldContext);
	
	ULimenCameraDistortionSetting();

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

	virtual void GlobalPostProcessFound(APostProcessVolume* PostProcess);
	
private:
};
