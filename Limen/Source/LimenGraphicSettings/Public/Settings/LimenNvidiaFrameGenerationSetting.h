// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenNvidiaFrameGenerationSetting.generated.h"


enum class UStreamlineDLSSGMode : uint8;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenNvidiaFrameGenerationSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	inline static const FString Enabled = TEXT("Enabled");
	inline static const FString Disabled = TEXT("Disabled");

	ULimenNvidiaFrameGenerationSetting();
	virtual void ApplyCurrentSetting() override;
	virtual bool CanEdit() const override;

protected:
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem) override;
	virtual void SetDefaults() override;

private:
	bool bCanEdit;
	
	static FString FormatNvidiaFrameGenerationSetting(const UStreamlineDLSSGMode Mode);
	static UStreamlineDLSSGMode UnFormatNvidiaFrameGenerationSetting(const FString& Selection);

	UFUNCTION()
	void OnFSRFrameGenerationSettingChanged(const ULimenSetting* Setting);
};
