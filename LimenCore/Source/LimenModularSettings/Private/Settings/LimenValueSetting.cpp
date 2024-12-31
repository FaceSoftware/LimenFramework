// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenValueSetting.h"

#include "BlueprintLibraries/LimenCoreStatics.h"


ULimenValueSetting::ULimenValueSetting(): DefaultSettingValue(0),
										  CurrentSettingValue(0),
										  PreviousSettingValue(0)
{
}

const TArray<float>& ULimenValueSetting::GetSettingValues() const
{
	return ValueRange;
}

float ULimenValueSetting::GetCurrentValue() const
{
	return CurrentSettingValue;
}

float ULimenValueSetting::GetPreviousValue() const
{
	return PreviousSettingValue;
}

bool ULimenValueSetting::IsValueValid(const float& Test)
{
	// Parent is pure virtual, do NOT call it!
	return Test >= ValueRange[0] && Test <= ValueRange[1];
}

bool ULimenValueSetting::CanEdit() const
{
	return true;
}

bool ULimenValueSetting::SetNewValue(const float& NewSelection)
{
	if (!TLimenEditableSetting::SetNewValue(NewSelection))
	{
		ULimenCoreStatics::LimenLog(this, FString::Printf(TEXT("Error setting '%s' to '%f'"), *GetDisplayName().ToString(), NewSelection), ELogType::Error);
		return false;
	}
 	
	PreviousSettingValue = CurrentSettingValue;
	CurrentSettingValue = NewSelection;
	StringValue = FString::Printf(TEXT("%f"), CurrentSettingValue);
	OnSettingUpdated.Broadcast(this);
	return true;
}

void ULimenValueSetting::SetDefaults()
{
	Super::SetDefaults();
	
	PreviousSettingValue = CurrentSettingValue;
}

void ULimenValueSetting::SetDefaultValue()
{
	PreviousSettingValue = CurrentSettingValue;
	CurrentSettingValue = DefaultSettingValue;
	StringValue = FString::Printf(TEXT("%f"), DefaultSettingValue);

	Super::SetDefaultValue();
}

void ULimenValueSetting::DataLoaded()
{
	Super::DataLoaded();
	
	CurrentSettingValue = FCString::Atof(*StringValue);
	PreviousSettingValue = CurrentSettingValue;
}

