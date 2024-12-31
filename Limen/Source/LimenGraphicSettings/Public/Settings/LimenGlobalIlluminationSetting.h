// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenGlobalIlluminationSetting.generated.h"


class APostProcessVolume;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenGlobalIlluminationSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenGlobalIlluminationSetting();

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

private:
	static FString FormatGISetting(const EDynamicGlobalIlluminationMethod::Type Method);
	static EDynamicGlobalIlluminationMethod::Type UnFormatGISetting(const FString& Selection);

	UFUNCTION()
	void GlobalPostProcessFound(APostProcessVolume* GlobalPostProcess);

	inline static const FString Lumen = TEXT("Lumen");
	inline static const FString ScreenSpace = TEXT("Screen Space");
};
