// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraModifiers/LimenGraphicalSettingsCameraModifier.h"

#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


void ULimenGraphicalSettingsCameraModifier::BindSubsystem(ULimenGraphicalSettingsSubsystem* GraphicalSettingsSubsystem)
{
	Subsystem = GraphicalSettingsSubsystem;
}

void ULimenGraphicalSettingsCameraModifier::ModifyPostProcess(const float DeltaTime, float& PostProcessBlendWeight,
															  FPostProcessSettings& PostProcessSettings)
{
	Super::ModifyPostProcess(DeltaTime, PostProcessBlendWeight, PostProcessSettings);
	
	PostProcessBlendWeight = 1.f;
	
	if (const auto* SubsystemPtr = Subsystem.Get())
	{
		SubsystemPtr->OnPostProcessSettingEvaluate.Broadcast(PostProcessSettings);
	}
}