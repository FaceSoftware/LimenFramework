// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenSensitivitySetting.h"

#include "..\..\..\LimenPlayers\Public\Characters\LimenPlayerCharacter.h"
#include "Kismet/GameplayStatics.h"


ULimenSensitivitySetting::ULimenSensitivitySetting()
{
	DevelopmentName = TEXT("setting_sensitivity");
	Category = FText::FromString(TEXT("General"));
	DisplayName = FText::FromString(TEXT("Mouse Sensitivity"));
	Description = FText::FromString(TEXT("Multiplier for the mouse movement."));
	bUseRecurrentAction = true;
}

void ULimenSensitivitySetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0);
	ValueRange.Push(10);
	DefaultSettingValue = 1.f;
}

void ULimenSensitivitySetting::RecurrentAction()
{
	APawn* Pawn = UGameplayStatics::GetPlayerPawn(this, 0);
	ALimenPlayerCharacter* Character = Cast<ALimenPlayerCharacter>(Pawn);
	PlayerCharacter = IsValid(Character) ? Character : nullptr;
}

bool ULimenSensitivitySetting::ShouldStopRecurrentAction()
{
	return PlayerCharacter != nullptr;
}

void ULimenSensitivitySetting::ActionSuccessful()
{
	Super::ActionSuccessful();

	FMouseParameters MouseParams = PlayerCharacter->GetMouseParameters();
	MouseParams.SensitivityY = GetCurrentValue();
	MouseParams.SensitivityX = GetCurrentValue();
	PlayerCharacter->SetMouseParameters(MouseParams);
}
