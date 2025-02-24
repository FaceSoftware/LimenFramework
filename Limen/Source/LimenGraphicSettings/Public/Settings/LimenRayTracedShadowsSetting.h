// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenRayTracedShadowsSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenRayTracedShadowsSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	static const FString Enabled;
	static const FString Disabled;

	ULimenRayTracedShadowsSetting();
	virtual bool CanEdit() const override;

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

private:
	TArray<TConsoleSetting<bool>> SettingsDescription;
	
	bool bCanEditOverride;

	UFUNCTION()
	void RayTracingSettingApplied(const ULimenSetting* Setting);
};
