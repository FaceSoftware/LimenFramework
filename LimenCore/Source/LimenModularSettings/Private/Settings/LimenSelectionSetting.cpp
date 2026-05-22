// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenSelectionSetting.h"

#include "BlueprintLibraries/LimenCoreStatics.h"


const FString ULimenSelectionSetting::EnabledString = TEXT("Enabled");
const FString ULimenSelectionSetting::DisabledString = TEXT("Disabled");

ULimenSelectionSetting::ULimenSelectionSetting()
{
	DefaultSelection = TEXT("DefaultSelection");
}

const TArray<FString>& ULimenSelectionSetting::GetSettingValues() const
{
	return PossibleSelections;
}

FString ULimenSelectionSetting::GetCurrentValue() const
{
	return CurrentSelection;
}

FString ULimenSelectionSetting::GetPreviousValue() const
{
	return PreviousSelection;
}

FString ULimenSelectionSetting::GetAppliedValue() const
{
	return AppliedSelection;
}

bool ULimenSelectionSetting::IsValueValid(const FString& Test)
{
	for (const auto& Selection : PossibleSelections)
	{
		if (Selection.Equals(Test))
		{
			return true;
		}
	}
	return false;
}

bool ULimenSelectionSetting::CanEdit() const
{
	return true;
}

bool ULimenSelectionSetting::SetNewValue(const FString& NewSelection)
{
	if (!TLimenEditableSetting<FString>::SetNewValue(NewSelection))
	{
		ULimenCoreStatics::LimenLog(this, FString::Printf(TEXT("Error setting '%s' to '%s'"), *GetDisplayName().ToString(), *NewSelection), ELogType::Error);
		return false;
	}
	
	SetIsApplied(false);
	PreviousSelection = CurrentSelection;
	CurrentSelection = NewSelection;
	OnSettingUpdated.Broadcast(this);
	return true;
}

void ULimenSelectionSetting::SetDefaults()
{
	Super::SetDefaults();
	
	PreviousSelection = CurrentSelection;
}

void ULimenSelectionSetting::SetDefaultValue()
{
	PreviousSelection = CurrentSelection;
	CurrentSelection = DefaultSelection;

	Super::SetDefaultValue();
}

void ULimenSelectionSetting::PostDataLoaded()
{
	PreviousSelection = CurrentSelection = AppliedSelection;	
	Super::PostDataLoaded();
}

bool ULimenSelectionSetting::IsEnabled() const
{
	return CurrentSelection == EnabledString;
}

void ULimenSelectionSetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);
	
	AppliedSelection = GetCurrentValue();
}
