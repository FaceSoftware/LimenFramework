// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenSensitivitySetting.h"

#include "TimerManager.h"
#include "Components/LimenMouseSensitivityComponent.h"
#include "Engine/World.h"
#include "PlayerController/LimenPlayerControllerBase.h"


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

void ULimenSensitivitySetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);

	const APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController == nullptr)
	{
		// Player controller should never be null. This means it was not initialized yet
		GetWorld()->GetTimerManager().SetTimerForNextTick([this, bUserRequest]
		{
			ApplyCurrentSetting(bUserRequest);
		});

		return;
	}

	auto* MouseSensitivityComponent = PlayerController->GetComponentByClass<ULimenMouseSensitivityComponent>();
	if (!ensure(MouseSensitivityComponent != nullptr))
	{
		return;
	}

	MouseSensitivityComponent->SetSensitivity(GetCurrentValue());
}
