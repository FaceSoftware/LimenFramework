// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenHintsSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGAMESETTINGS_API ULimenHintsSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenHintsSetting();
	virtual void ApplyCurrentSetting(bool bUserRequest) override;

protected:
	virtual void SetDefaults() override;

private:
	static FString FormatHintsSetting(const bool bEnabled);
	static bool UnFormatHintsSetting(const FString& Selection);

	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");
};
