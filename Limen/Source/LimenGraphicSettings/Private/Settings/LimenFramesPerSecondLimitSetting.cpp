// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenFramesPerSecondLimitSetting.h"

#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"

ULimenFramesPerSecondLimitSetting::ULimenFramesPerSecondLimitSetting()
{
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("FPS Limit"));
	Description = FText::FromString(TEXT("Limits the amount of frames displayed each second."));
}

void ULimenFramesPerSecondLimitSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	float Limit = FCString::Atof(*GetCurrentValue());
	if (FMath::IsNearlyEqualByULP(Limit, 0, 1))
	{
		// If we set 0 (standard way of setting as unlimited) it defaults to 120, I don't know why...
		// just set to an enormous value that is most likely never going to be hit
		Limit = 1000.f;
	}
	GEngine->GetGameUserSettings()->SetFrameRateLimit(Limit);
	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void ULimenFramesPerSecondLimitSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Reserve(5);
	PossibleSelections.Push("30");
	PossibleSelections.Push("60");
	PossibleSelections.Push("120");
	PossibleSelections.Push("144");
	PossibleSelections.Push("Unlimited");
	DefaultSelection = TEXT("60");
}
