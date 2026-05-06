// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenValueSetting.h"

#include "BlueprintLibraries/LimenCoreStatics.h"


ULimenValueSetting::ULimenValueSetting() : DefaultSettingValue(0),
										   DecimalsDisplayed(0),
										   CurrentSettingValue(0),
										   PreviousSettingValue(0),
										   MinValuePerChange(1.f)
{
}

float ULimenValueSetting::GetMinValuePerChange() const
{
	return MinValuePerChange;
}

int32 ULimenValueSetting::GetDecimalCount() const
{
	return DecimalsDisplayed;
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

float ULimenValueSetting::GetAppliedValue() const
{
	return AppliedSettingValue;
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
 	
	SetIsApplied(false);
	PreviousSettingValue = CurrentSettingValue;
	CurrentSettingValue = NewSelection;
	OnSettingUpdated.Broadcast(this);
	return true;
}

void ULimenValueSetting::SetDefaultValue()
{
	PreviousSettingValue = CurrentSettingValue;
	CurrentSettingValue = DefaultSettingValue;
	StringValue = FString::Printf(TEXT("%f"), DefaultSettingValue);

	Super::SetDefaultValue();
}

void ULimenValueSetting::SetDefaults()
{
	Super::SetDefaults();
	
	PreviousSettingValue = CurrentSettingValue;
	MinValuePerChange = FMath::Pow(10.f, -DecimalsDisplayed);
}

void ULimenValueSetting::PostDataLoaded()
{
	PreviousSettingValue = CurrentSettingValue = AppliedSettingValue = FCString::Atof(*StringValue);
	Super::PostDataLoaded();
}

void ULimenValueSetting::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);
	
	AppliedSettingValue = GetCurrentValue();
	StringValue = FString::Printf(TEXT("%f"), AppliedSettingValue);
}
