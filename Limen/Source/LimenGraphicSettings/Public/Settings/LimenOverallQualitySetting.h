// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenOverallQualitySetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenOverallQualitySetting : public ULimenSelectionSetting
{
	GENERATED_BODY()
	
public:
	ULimenOverallQualitySetting();

protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

private:
	static FString FormatReflectionSetting(const int32 Method);
	static int32 UnFormatReflectionSetting(const FString& Selection);

	inline static const FString High = TEXT("High");
	inline static const FString Ultra = TEXT("Ultra");
	inline static const FString Cinematic = TEXT("Cinematic");
};
