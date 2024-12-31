// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenReflectionsMethodSetting.generated.h"

class APostProcessVolume;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenReflectionsMethodSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenReflectionsMethodSetting();

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

private:
	static FString FormatReflectionSetting(const EReflectionMethod::Type Method);
	static EReflectionMethod::Type UnFormatReflectionSetting(const FString& Selection);

	UFUNCTION()
	void GlobalPostProcessFound(APostProcessVolume* GlobalPostProcess);

	inline static const FString Lumen = TEXT("Lumen");
	inline static const FString ScreenSpace = TEXT("Screen Space");
};
