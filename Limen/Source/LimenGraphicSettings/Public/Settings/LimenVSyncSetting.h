// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenVSyncSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenVSyncSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenVSyncSetting();

protected:
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;

private:
	static FString FormatVSyncSetting(const bool bIsEnabled);
	static bool UnFormatVSyncSetting(const FString& Selection);

	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");
};
