// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintAsyncActions/LimenTickCheck.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenHintsSetting.generated.h"

class ALimenHintsManager;
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
	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");
	static constexpr int32 MaxRetries = 100;
	
	static FString FormatHintsSetting(const bool bEnabled);
	static bool UnFormatHintsSetting(const FString& Selection);

	TWeakObjectPtr<ALimenHintsManager> HintsManager;
	int32 Retries;
	
	FLimenTickCheck HintsManagerSpawn;
};
