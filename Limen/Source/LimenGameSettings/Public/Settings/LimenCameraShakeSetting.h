// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenCameraShakeSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGAMESETTINGS_API ULimenCameraShakeSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenCameraShakeSetting();
	
protected:
	virtual void SetDefaults() override;

	virtual void RecurrentAction() override;
	virtual bool ShouldStopRecurrentAction() override;
	virtual void ActionSuccessful() override;

private:
	UPROPERTY()
	TObjectPtr<APawn> PlayerPawn;
	
	static FString FormatCameraShakeSetting(const bool bEnabled);
	static bool UnFormatCameraShakeSetting(const FString& Selection);

	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");
};
