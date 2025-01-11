// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenSetting.h"

#include "LogMacros/LimenLogMacros.h"
#include "BlueprintAsyncActions/LimenRecurrentAction.h"


ULimenSetting::ULimenSetting()
{
	DevelopmentName = TEXT("DefaultSettingDevelopmentName");
	Category = FText::FromString(TEXT("DefaultCategory"));
	DisplayName = FText::FromString(TEXT("DefaultSettingDisplayName"));
	Description = FText::FromString(TEXT("Override this in a child class of ULimenSetting"));
	bUseRecurrentAction = false;
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

	LIMEN_LOG(LogLimen, Log, this, "Applying setting %s", *GetDevelopmentName().ToString());
}

void ULimenSetting::SetDefaultValue()
{
	OnSettingUpdated.Broadcast(this);
}

void ULimenSetting::InitializeSetting()
{
	if (bHasInitialized)
	{
		return;
	}
	
	SetDefaults();
	BP_SetDefaults();
	SetDefaultValue();
	ApplySetting();
	bHasInitialized = true;
	bShouldLoadData = true;
	LIMEN_LOG(LogLimen, Log, this, "Initialized setting %s", *GetDevelopmentName().ToString());
}

void ULimenSetting::SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem)
{
}

void ULimenSetting::RecurrentAction()
{
}

bool ULimenSetting::ShouldStopRecurrentAction()
{
	return false;
}

void ULimenSetting::ActionSuccessful()
{
	DestroyRecurrentActionObject();
}

void ULimenSetting::ActionUnsuccessful()
{
	DestroyRecurrentActionObject();
}

void ULimenSetting::SetDefaults()
{
}

void ULimenSetting::ApplyCurrentSetting(const bool bUserRequest)
{
	if (!bUseRecurrentAction)
	{
		return;
	}

	if (RecurrentActionObject != nullptr)
	{
		return;
	}

	FRecurrentActionDelegate RecurrentActionDelegate;
	RecurrentActionDelegate.BindDynamic(this, &ThisClass::RecurrentAction);
	FRecurrentActionStopCondition RecurrentActionStopCondition;
	RecurrentActionStopCondition.BindDynamic(this, &ThisClass::ShouldStopRecurrentAction);

	RecurrentActionObject = ULimenRecurrentAction::StartRecurrentAction(GetWorld(), RecurrentActionDelegate, 0, .1, RecurrentActionStopCondition);
	RecurrentActionObject->OnSuccess.AddDynamic(this, &ThisClass::ActionSuccessful);
	RecurrentActionObject->OnFailure.AddDynamic(this, &ThisClass::ActionUnsuccessful);
	RecurrentActionObject->Activate();
}

void ULimenSetting::DataLoaded()
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

void ULimenSetting::DestroyRecurrentActionObject()
{
	RecurrentActionObject->ConditionalBeginDestroy();
	RecurrentActionObject = nullptr;
}
