// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "LimenGraphicalSettingsSubsystem.generated.h"

class ULimenGraphicalSettingsCameraModifier;
class ULimenGraphicalSettingsSubsystem;
class ULimenGraphicalSettingsDeveloperSettings;


/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenGraphicalSettingsSubsystem : public ULimenModularSettingsSubsystem
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FGlobalPostProcessFound, APostProcessVolume* /* GlobalPostProcess */);
	DECLARE_MULTICAST_DELEGATE_OneParam(FPostProcessSettingsDelegate, FPostProcessSettings& /* Settings */);

public:
	FGlobalPostProcessFound OnGlobalPostProcessFound;
	FPostProcessSettingsDelegate OnPostProcessSettingEvaluate;
	
	ULimenGraphicalSettingsSubsystem();
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

protected:
	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;
	virtual void LoadDefaultSettingsList() override;

private:
	TWeakObjectPtr<const ULimenGraphicalSettingsDeveloperSettings> SubsystemSettings;
	TWeakObjectPtr<ULimenGraphicalSettingsCameraModifier> CameraModifier;
};
