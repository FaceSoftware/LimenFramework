// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenResolutionSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenResolutionSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenResolutionSetting();
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem) override;
	virtual bool CanEdit() const override;

protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

private:
	static FString FormatResolution(const FScreenResolutionRHI& InResolution);
	static FString FormatResolution(const uint32 Width, const uint32 Height);
	static FIntPoint DeFormatResolution(const FString& InResolutionString);

	FString CurrentWindowModeSetting;
	FString LastWindowModeSetting;
	
	UFUNCTION()
	void OnWindowModeUpdated(const ULimenSetting* Setting);
};
