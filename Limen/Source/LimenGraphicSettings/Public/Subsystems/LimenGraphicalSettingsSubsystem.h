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
	virtual void PostWorldInitialization(UWorld* World, const UWorld::InitializationValues InitValues) override;

private:
	UPROPERTY()
	TObjectPtr<const ULimenGraphicalSettingsDeveloperSettings> SubsystemSettings;
	UPROPERTY()
	TObjectPtr<APostProcessVolume> GlobalPostProcess;

	TUniquePtr<FLimenTickCheck> FindGlobalPostProcess;

	static APostProcessVolume* FindGlobalPostProcessVolume(const UWorld* World, FName Tag);
};
