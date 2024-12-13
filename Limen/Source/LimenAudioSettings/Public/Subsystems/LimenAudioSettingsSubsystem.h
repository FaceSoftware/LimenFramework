// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "LimenAudioSettingsSubsystem.generated.h"

class ULimenAudioDeveloperSettings;
/**
 * 
 */
UCLASS()
class LIMENAUDIOSETTINGS_API ULimenAudioSettingsSubsystem : public ULimenModularSettingsSubsystem
{
	GENERATED_BODY()

public:
	ULimenAudioSettingsSubsystem();

protected:
	virtual void LoadDefaultSettingsList() override;

private:
	UPROPERTY()
	TObjectPtr<const ULimenAudioDeveloperSettings> SubsystemSettings;
};
