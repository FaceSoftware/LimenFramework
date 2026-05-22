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
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

private:
	static FString FormatReflectionSetting(const int32 Method);
	static int32 UnFormatReflectionSetting(const FString& Selection);

	inline static const FString Low = TEXT("Low");
	inline static const FString Medium = TEXT("Medium");
	inline static const FString High = TEXT("High");
	inline static const FString Epic = TEXT("Epic");
	inline static const FString Cinematic = TEXT("Cinematic");
};
