// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenHintsSetting.h"

#include "TimerManager.h"
#include "BlueprintAsyncActions/LimenTickCheck.h"
#include "GameMode/LimenGameModeBase.h"
#include "GameplayManagers/LimenHintsManager.h"
#include "HUDs/LimenHUD.h"
#include "Kismet/GameplayStatics.h"
#include "LimenNotifications/Public/Components/LimenNotificationComponent.h"


ULimenHintsSetting::ULimenHintsSetting()
{
	Retries = 0;
	
	DevelopmentName = TEXT("setting_tutorialmsg");
	Category = FText::FromString(TEXT("General"));
	DisplayName = FText::FromString(TEXT("Tutorial Messages"));
	Description = FText::FromString(TEXT("Whether or not to display tutorial messages."));
}

void ULimenHintsSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	HintsManagerSpawn = FLimenTickCheck(GetWorld());
	HintsManagerSpawn.AddLambda([this]
	{
		if (HintsManager = ALimenGameplayManager::GetGameplayManager<ALimenHintsManager>(this); HintsManager.IsValid())
		{
			UnFormatHintsSetting(GetCurrentValue()) ? HintsManager->EnableHints() : HintsManager->DisableHints();
			return true;
		}

		return false;
	});
}

void ULimenHintsSetting::SetDefaults()
{
	Super::SetDefaults();

	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);
	DefaultSelection = Enabled;
}

FString ULimenHintsSetting::FormatHintsSetting(const bool bEnabled)
{
	return bEnabled ? Enabled : Disabled;
}

bool ULimenHintsSetting::UnFormatHintsSetting(const FString& Selection)
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
