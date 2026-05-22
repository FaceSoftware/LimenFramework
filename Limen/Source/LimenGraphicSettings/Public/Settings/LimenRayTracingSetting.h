// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenRayTracingSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenRayTracingSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()
	
public:
	static const FString Enabled;
	static const FString Disabled;
	
	ULimenRayTracingSetting();
	virtual bool CanEdit() const override;

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

private:
	TArray<TConsoleSetting<bool>> SettingsDescription;
};
