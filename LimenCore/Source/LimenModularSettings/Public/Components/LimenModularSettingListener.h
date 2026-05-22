// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Settings/LimenSetting.h"
#include "LimenModularSettingListener.generated.h"

class ULimenSetting;
class ULimenModularSettingsSubsystem;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENMODULARSETTINGS_API ULimenModularSettingListener : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSettingUpdated OnSettingUpdated;
	
	ULimenModularSettingListener();
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

protected:
	TArray<TSoftClassPtr<ULimenSetting>> SettingsToListen;

	UFUNCTION()
	virtual void SettingUpdated(const ULimenSetting* Setting);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess))
	TWeakObjectPtr<ULimenModularSettingsSubsystem> ModularSettingsSubsystem;
};
