// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenVSyncSetting.h"

#include "Engine/Engine.h"
#include "GameFramework/GameUserSettings.h"


ULimenVSyncSetting::ULimenVSyncSetting()
{
	DevelopmentName = TEXT("setting_vsync");
	Category = FText::FromString(TEXT("Display"));
	DisplayName = FText::FromString(TEXT("VSync"));
	Description = FText::FromString(TEXT("Refreshes the screen's pixels in sync to avoid tearing."));
}

void ULimenVSyncSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	Super::ApplyCurrentSetting();

	GEngine->GetGameUserSettings()->SetVSyncEnabled(UnFormatVSyncSetting(GetCurrentValue()));
	GEngine->GetGameUserSettings()->ApplySettings(false);
}

void ULimenVSyncSetting::SetDefaults()
{
	Super::SetDefaults();
	
	PossibleSelections.Reserve(2);
	PossibleSelections.Push(Enabled);
	PossibleSelections.Push(Disabled);

	DefaultSelection = Enabled;
}

FString ULimenVSyncSetting::FormatVSyncSetting(const bool bIsEnabled)
{
	return bIsEnabled ? Enabled : Disabled;
}

bool ULimenVSyncSetting::UnFormatVSyncSetting(const FString& Selection)
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
	return false;
}
