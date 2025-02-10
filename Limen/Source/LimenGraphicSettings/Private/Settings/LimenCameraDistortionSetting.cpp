// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenCameraDistortionSetting.h"

#include "Camera/CameraComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Subsystems/LimenGraphicalSettingsSubsystem.h"


void ULimenCameraDistortionSetting::SetCameraDistortionState(UObject* WorldContext, const bool bEnable)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::Assert);
	const ULimenGraphicalSettingsSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	ULimenCameraDistortionSetting* Setting = Subsystem->GetItem<ULimenCameraDistortionSetting>();
	Setting->SetNewValue(bEnable ? Enabled : Disabled);
}

bool ULimenCameraDistortionSetting::GetCameraDistortionState(UObject* WorldContext)
{
	const UWorld* World = GEngine->GetWorldFromContextObject(WorldContext, EGetWorldErrorMode::Assert);
	const ULimenGraphicalSettingsSubsystem* Subsystem = World->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	const ULimenCameraDistortionSetting* Setting = Subsystem->GetItem<ULimenCameraDistortionSetting>();
	return Setting->GetCurrentValue() == Enabled;
}

ULimenCameraDistortionSetting::ULimenCameraDistortionSetting()
{
	DevelopmentName = TEXT("setting_postprocessmaterials");
	Category = FText::FromString(TEXT("Post Process FX"));
	DisplayName = FText::FromString(TEXT("Camera Distortion"));
	Description = FText::FromString(TEXT("Controls the VHS camera distortion effect."));
}

void ULimenCameraDistortionSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();
	
	auto* GraphicalSettings = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGraphicalSettingsSubsystem>();
	if (auto* GlobalPostProcess = GraphicalSettings->GetGlobalPostProcess(); GlobalPostProcess != nullptr)
	{
		GlobalPostProcessFound(GlobalPostProcess);
	}
	else
	{
		GraphicalSettings->OnGlobalPostProcessFound.AddUObject(this, &ThisClass::GlobalPostProcessFound);
	}
}

void ULimenCameraDistortionSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Reserve(2);
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = Enabled;
}

void ULimenCameraDistortionSetting::GlobalPostProcessFound(APostProcessVolume* PostProcess)
{
	for (auto& Blendable : PostProcess->Settings.WeightedBlendables.Array)
	{
		Blendable.Weight = GetCurrentValue() == Enabled ? 1 : 0;
	}
}
