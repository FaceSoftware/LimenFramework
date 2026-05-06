// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "LimenGraphicalSettingsCameraModifier.generated.h"

class ULimenGraphicalSettingsSubsystem;


/**
 * 
 */
UCLASS()
class ULimenGraphicalSettingsCameraModifier : public UCameraModifier
{
	GENERATED_BODY()

public:
	void BindSubsystem(ULimenGraphicalSettingsSubsystem* GraphicalSettingsSubsystem);
	
protected:
	virtual void ModifyPostProcess(float DeltaTime, float& PostProcessBlendWeight, FPostProcessSettings& PostProcessSettings) override;
	
private:
	TWeakObjectPtr<ULimenGraphicalSettingsSubsystem> Subsystem;
};
