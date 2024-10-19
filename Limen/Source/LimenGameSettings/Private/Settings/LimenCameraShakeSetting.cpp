// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenCameraShakeSetting.h"

#include "Components/LimenVelocityBasedCameraShakeComponent.h"
#include "Kismet/GameplayStatics.h"


ULimenCameraShakeSetting::ULimenCameraShakeSetting()
{
	DevelopmentName = TEXT("setting_camerashakes");
	Category = FText::FromString(TEXT("Gameplay"));
	DisplayName = FText::FromString(TEXT("Camera Shakes"));
	Description = FText::FromString(TEXT("Whether or not to play camera shakes for standing, walking, running, etc."));
	bUseRecurrentAction = true;
}

void ULimenCameraShakeSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = Enabled;
}

void ULimenCameraShakeSetting::RecurrentAction()
{
	Super::RecurrentAction();
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
	PlayerPawn = IsValid(Pawn) ? Pawn : nullptr;
}

bool ULimenCameraShakeSetting::ShouldStopRecurrentAction()
{
	return PlayerPawn != nullptr;
}

void ULimenCameraShakeSetting::ActionSuccessful()
{
	Super::ActionSuccessful();

	auto* CameraShakeComponent = PlayerPawn->GetComponentByClass<ULimenVelocityBasedCameraShakeComponent>();
	if (CameraShakeComponent != nullptr)
	{
		if (UnFormatCameraShakeSetting(GetCurrentValue()))
		{
			CameraShakeComponent->Activate(true);
		}
		else
		{
			CameraShakeComponent->Deactivate();
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
