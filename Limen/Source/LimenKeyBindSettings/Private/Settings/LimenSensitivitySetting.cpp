// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenSensitivitySetting.h"

#include "Components/LimenMouseSensitivityComponent.h"
#include "Engine/World.h"
#include "Subsystems/LimenModularSettingsSubsystem.h"


ULimenSensitivitySetting::ULimenSensitivitySetting()
{
	DevelopmentName = TEXT("setting_sensitivity");
	Category = FText::FromString(TEXT("Mouse"));
	DisplayName = FText::FromString(TEXT("Mouse Sensitivity"));
	Description = FText::FromString(TEXT("Multiplier for the mouse movement."));
	DecimalsDisplayed = 1;
}

void ULimenSensitivitySetting::SetDefaults()
{
	Super::SetDefaults();

	ValueRange.Reserve(2);
	ValueRange.Push(0);
	ValueRange.Push(10);
	DefaultSettingValue = 1.f;
}

void ULimenSensitivitySetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);

	const APlayerController* PlayerController = GetOwnerSubsystem()->GetCurrentPlayerController();
	if (!PlayerController) { return; }

	auto* MouseSensitivityComponent = PlayerController->GetComponentByClass<ULimenMouseSensitivityComponent>();
	if (!MouseSensitivityComponent) { return; }
	MouseSensitivityComponent->SetSensitivity(GetCurrentValue());
}
