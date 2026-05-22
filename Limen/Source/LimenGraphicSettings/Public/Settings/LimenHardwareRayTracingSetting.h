// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenHardwareRayTracingSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenHardwareRayTracingSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	static const FString Software;
	static const FString Hardware;

	ULimenHardwareRayTracingSetting();
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
