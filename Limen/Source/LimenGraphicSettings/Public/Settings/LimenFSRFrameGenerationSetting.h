// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenFSRFrameGenerationSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenFSRFrameGenerationSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenFSRFrameGenerationSetting();

	inline static FString Enabled = TEXT("Enabled");
	inline static FString Disabled = TEXT("Disabled");

	virtual bool CanEdit() const override;

protected:
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem) override;
	virtual void SetDefaults() override;
	virtual void ApplyCurrentSetting() override;

private:
	bool bCanEdit;
	
	UFUNCTION()
	void OnNvidiaFrameGenerationSettingChanged(const ULimenSetting* Setting);
};
