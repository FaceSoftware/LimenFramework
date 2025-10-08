// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenSetting.h"

#include "LogMacros/LimenLogMacros.h"


const EConsoleVariableFlags ULimenSetting::ConsoleVariableUserSettingFlag = EConsoleVariableFlags::ECVF_SetByGameOverride;

ULimenSetting::ULimenSetting()
{
	DevelopmentName = TEXT("DefaultSettingDevelopmentName");
	Category = FText::FromString(TEXT("DefaultCategory"));
	DisplayName = FText::FromString(TEXT("DefaultSettingDisplayName"));
	Description = FText::FromString(TEXT("Override this in a child class of ULimenSetting"));
	bHasInitialized = false;
	bShouldLoadData = false;
}

const FText& ULimenSetting::GetDescription() const
{
	return Description;
}

void ULimenSetting::ApplySetting(const bool bUserRequest)
{
	ApplyCurrentSetting(bUserRequest);
	OnSettingApplied.Broadcast(this);

	LIMEN_LOG(LogLimen, Log, this, TEXT("Applying setting %s"), *GetDevelopmentName().ToString());
}

void ULimenSetting::SetDefaultValue()
{
	OnSettingUpdated.Broadcast(this);
}

void ULimenSetting::InitializeSetting(ULimenModularSettingsSubsystem* InOwnerSubsystem)
{
	if (bHasInitialized)
	{
		return;
	}

	check(InOwnerSubsystem != nullptr)
	OwnerSubsystem = InOwnerSubsystem;
	
	SetDefaults();
	BP_SetDefaults();
	SetDefaultValue();
	ApplySetting();
	bHasInitialized = true;
	bShouldLoadData = true;

	LIMEN_LOG(LogLimen, Log, this, TEXT("Initialized setting %s"), *GetDevelopmentName().ToString());
}

void ULimenSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
}

void ULimenSetting::SetDefaults()
{
}

void ULimenSetting::ApplyCurrentSetting(bool bUserRequest)
{
}

void ULimenSetting::PostDataLoaded()
{
	bShouldLoadData = false;
	ApplyCurrentSetting();
}

bool ULimenSetting::ShouldLoadData() const
{
	if (!Super::ShouldLoadData())
	{
		return false;
	}

	return bShouldLoadData;
}
