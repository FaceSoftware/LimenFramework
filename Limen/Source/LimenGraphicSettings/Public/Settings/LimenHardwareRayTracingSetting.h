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
	ULimenHardwareRayTracingSetting();
	virtual bool CanEdit() const override;

protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

private:
	static FString FormatHardwareRayTracingSetting(const bool bEnabled);
	static bool UnFormatHardwareRayTracingSetting(const FString& Selection);

	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");
};
