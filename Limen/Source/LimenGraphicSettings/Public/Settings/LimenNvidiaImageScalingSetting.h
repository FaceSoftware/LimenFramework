// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenNvidiaImageScalingSetting.generated.h"


enum class UNISMode : uint8;
enum class UNISSupport : uint8;

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenNvidiaImageScalingSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()


public:
	inline static const FString Disabled = TEXT("Disabled");
	inline static const FString UltraQuality = TEXT("Ultra Quality");
	inline static const FString Quality = TEXT("Quality");
	inline static const FString Balanced = TEXT("Balanced");
	inline static const FString Performance = TEXT("Performance");	

	ULimenNvidiaImageScalingSetting();
	virtual bool CanEdit() const override;

protected:
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem) override;
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

private:
	static FString FormatNisSetting(const UNISMode Mode);
	static UNISMode UnFormatNisSetting(const FString& Selection);

	UFUNCTION()
	void OnFsrModeChanged(const ULimenSetting* SettingChanged);

	bool bCanEdit;
};
