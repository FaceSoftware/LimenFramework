// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/PostProcessVolume.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "LimenGraphicalSettingsSubsystem.generated.h"

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
	
	APostProcessVolume* GetGlobalPostProcess();

protected:
	virtual void PostWorldInitialization(UWorld* World, const UWorld::InitializationValues InitValues) override;

private:
	UPROPERTY()
	TObjectPtr<const ULimenGraphicalSettingsDeveloperSettings> SubsystemSettings;
	UPROPERTY()
	TObjectPtr<ULimenRecurrentAction> FindGlobalPostProcessAction;
	UPROPERTY()
	TObjectPtr<APostProcessVolume> GlobalPostProcess;
	FTimerHandle FindGlobalPostProcessHandle;

	UFUNCTION()
	bool StopRecurrentAction();
	
	static APostProcessVolume* FindGlobalPostProcessVolume(const UWorld* World, const FName Tag);
	UFUNCTION()
	void FindGlobalPostProcessVolumeWrapper();
	UFUNCTION()
	void GlobalPostProcessFound();
};
