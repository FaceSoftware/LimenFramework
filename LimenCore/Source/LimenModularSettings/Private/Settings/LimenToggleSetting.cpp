// Copyright Face Software. All Rights Reserved.


#include "Settings/LimenToggleSetting.h"

#include "BlueprintLibraries/LimenCoreStatics.h"

ULimenToggleSetting::ULimenToggleSetting() : bDefaultSettingState(false), bCurrentSettingState(false)
{
	PossibleStates.Push(false);
	PossibleStates.Push(true);
}

const TArray<bool>& ULimenToggleSetting::GetSettingValues() const
{
	/// I know this is a laughing matter but I guess it's good to keep tradition with the other classes.
	/// Probably no one will ever use this function but here it is just in case.
	return PossibleStates;
}

const bool& ULimenToggleSetting::GetCurrentValue() const
{
	return bCurrentSettingState;
}

bool ULimenToggleSetting::IsValueValid(const bool& Test)
{
	// Parent is pure virtual, do NOT call it!
	return true;
}

bool ULimenToggleSetting::CanEdit() const
{
	return true;
}

bool ULimenToggleSetting::SetNewValue(const bool& NewSelection)
{
	if (!TLimenEditableSetting<bool>::SetNewValue(NewSelection))
	{
		ULimenCoreStatics::LimenLog(this, FString::Printf(TEXT("Error setting '%s' to '%d'"), *GetDisplayName().ToString(), NewSelection), ELogType::Error);
		return false;
	}
 	
	bCurrentSettingState = NewSelection;
	OnSettingUpdated.Broadcast(this);
	return true;
}

void ULimenToggleSetting::SetDefaultValue()
{
	verify(SetNewValue(bDefaultSettingState));
}
