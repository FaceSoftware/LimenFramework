// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintAsyncActions/LimenTickCheck.h"
#include "Engine/PostProcessVolume.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "LimenGraphicalSettingsSubsystem.generated.h"

class FLimenTickCheck;
class ULimenRecurrentAction;
class ULimenGraphicalSettingsDeveloperSettings;

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenGraphicalSettingsSubsystem : public ULimenModularSettingsSubsystem
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_OneParam(FGlobalPostProcessFound, APostProcessVolume* /* GlobalPostProcess */);

public:
	FGlobalPostProcessFound OnGlobalPostProcessFound;
	
	ULimenGraphicalSettingsSubsystem();
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void LoadDefaultSettingsList() override;
	
	APostProcessVolume* GetGlobalPostProcess() const;

protected:
	virtual void WorldInitializedActors(const FActorsInitializedParams& InitParams) override;

private:
	TWeakObjectPtr<const ULimenGraphicalSettingsDeveloperSettings> SubsystemSettings;
	TWeakObjectPtr<APostProcessVolume> GlobalPostProcess;
	FTimerHandle FindPostProcessHandle;

	void FindGlobalPostProcessVolume(const UWorld* World, const FName& Tag);
};
