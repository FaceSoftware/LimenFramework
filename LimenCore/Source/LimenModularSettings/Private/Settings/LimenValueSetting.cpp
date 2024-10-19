// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenValueSetting.h"

#include "BlueprintLibraries/LimenCoreStatics.h"


ULimenValueSetting::ULimenValueSetting()
{
	CurrentSettingValue = 0.f;
	DefaultSettingValue = 0.f;
}

const TArray<float>& ULimenValueSetting::GetSettingValues() const
{
	return ValueRange;
}

const float& ULimenValueSetting::GetCurrentValue() const
{
	return CurrentSettingValue;
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
	if (!TLimenEditableSetting<float>::SetNewValue(NewSelection))
	{
		ULimenCoreStatics::LimenLog(this, FString::Printf(TEXT("Error setting '%s' to '%f'"), *GetDisplayName().ToString(), NewSelection), ELogType::Error);
		return false;
	}
 	
	CurrentSettingValue = NewSelection;
	StringValue = FString::Printf(TEXT("%f"), CurrentSettingValue);
	return true;
}

void ULimenValueSetting::SetDefaultValue()
{
	Super::SetDefaultValue();
	
	if (FMath::IsNearlyEqualByULP(CurrentSettingValue, DefaultSettingValue, 2))
	{
		return;
	}
	
	CurrentSettingValue = DefaultSettingValue;
	StringValue = FString::Printf(TEXT("%f"), DefaultSettingValue);
}

void ULimenValueSetting::DataLoaded()
{
	CurrentSettingValue = FCString::Atof(*StringValue);
	Super::DataLoaded();
}

