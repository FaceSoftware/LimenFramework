// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenCameraShakeSetting.h"

#include "EngineUtils.h"
#include "Components/LimenCameraShakeComponent.h"
#include "Components/LimenCameraTiltComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


ULimenCameraShakeSetting::ULimenCameraShakeSetting()
{
	DevelopmentName = TEXT("setting_camerashakes");
	Category = FText::FromString(TEXT("Gameplay"));
	DisplayName = FText::FromString(TEXT("Camera Shakes"));
	Description = FText::FromString(TEXT("Whether or not to play camera shakes."));
	bUseRecurrentAction = false;
}

void ULimenCameraShakeSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = Enabled;
}

void ULimenCameraShakeSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		TArray<ULimenCameraTiltComponent*> TiltCameras;
		It->GetComponents<ULimenCameraTiltComponent>(TiltCameras, true);
		for (ULimenCameraTiltComponent*& Camera : TiltCameras)
		{
			Camera->SetTiltEnabled(UnFormatCameraShakeSetting(GetCurrentValue()));
		}
		
		TArray<ULimenCameraShakeComponent*> CameraShakes;
		It->GetComponents<ULimenCameraShakeComponent>(CameraShakes, true);
		for (ULimenCameraShakeComponent* const& CameraShake : CameraShakes)
		{
			CameraShake->SetActive(UnFormatCameraShakeSetting(GetCurrentValue()), true);
		}
	}
}

FString ULimenCameraShakeSetting::FormatCameraShakeSetting(const bool bEnabled)
{
	return bEnabled ? Enabled : Disabled;
}

bool ULimenCameraShakeSetting::UnFormatCameraShakeSetting(const FString& Selection)
{
	if (Selection.Compare(Enabled) == 0)
	{
		return true;
	}
	if (Selection.Compare(Disabled) == 0)
	{
		return false;
	}

	checkNoEntry();
	return {};
}
