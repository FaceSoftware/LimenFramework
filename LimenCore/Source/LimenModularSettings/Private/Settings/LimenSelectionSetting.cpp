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

const FString& ULimenSelectionSetting::GetCurrentValue() const
{
	return CurrentSelection;
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
 	
	CurrentSelection = NewSelection;
	return true;
}

void ULimenSelectionSetting::SetDefaultValue()
{
	if (DefaultSelection == TEXT("DefaultSelection"))
	{
		return;
	}
	
	CurrentSelection = DefaultSelection;
}

void ULimenSelectionSetting::DataLoaded()
{
	Super::DataLoaded();
}
