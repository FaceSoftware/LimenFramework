// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenHintsSetting.h"

#include "GameplayManagers/LimenHintsManager.h"
#include "HUDs/LimenHUD.h"
#include "Kismet/GameplayStatics.h"
#include "LimenNotifications/Public/Components/LimenNotificationComponent.h"

ULimenHintsSetting::ULimenHintsSetting()
{
	DevelopmentName = TEXT("setting_tutorialmsg");
	Category = FText::FromString(TEXT("General"));
	DisplayName = FText::FromString(TEXT("Tutorial Messages"));
	Description = FText::FromString(TEXT("Whether or not to display tutorial messages."));
}

void ULimenHintsSetting::ApplyCurrentSetting()
{
	Super::ApplyCurrentSetting();

	if (const auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		PlayerController == nullptr)
	{
		return;
	}

	if (ALimenHintsManager* HintsManager = Cast<ALimenHintsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ALimenHintsManager::StaticClass()));
		HintsManager != nullptr)
	{
		UnFormatHintsSetting(GetCurrentValue()) ? HintsManager->EnableHints() : HintsManager->DisableHints();
	}
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
