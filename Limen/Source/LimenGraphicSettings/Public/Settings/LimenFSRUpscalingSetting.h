// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenFSRUpscalingSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenFSRUpscalingSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	inline static const FString Disabled = TEXT("Disabled");
	inline static const FString Quality = TEXT("Quality");
	inline static const FString Balanced = TEXT("Balanced");
	inline static const FString Performance = TEXT("Performance");	
	inline static const FString UltraPerformance = TEXT("Ultra Performance");	

	ULimenFSRUpscalingSetting();

	virtual bool CanEdit() const override;

protected:
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem) override;
	virtual void SetDefaults() override;
	virtual void ApplyCurrentSetting() override;

private:
	bool bCanEdit;
	
	static FString FormatFsrSetting(const uint8 InValue);
	static uint8 UnFormatFsrSetting(const FString& InValue);

	UFUNCTION()
	void OnNisSettingChanged(const ULimenSetting* Setting);
};
