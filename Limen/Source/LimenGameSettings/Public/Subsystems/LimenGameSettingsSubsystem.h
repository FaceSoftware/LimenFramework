// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"
#include "LimenGameSettingsSubsystem.generated.h"


class ULimenGameSettingsDeveloperSettings;

/**
 * 
 */
UCLASS()
class LIMENGAMESETTINGS_API ULimenGameSettingsSubsystem : public ULimenModularSettingsSubsystem
{
	GENERATED_BODY()

protected:
	ULimenGameSettingsSubsystem();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void LoadDefaultSettingsList() override;

private:
	UPROPERTY()
	TObjectPtr<const ULimenGameSettingsDeveloperSettings> SubsystemSettings;
};
