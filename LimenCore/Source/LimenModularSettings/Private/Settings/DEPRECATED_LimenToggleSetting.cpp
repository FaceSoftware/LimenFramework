// Copyright Face Software. All Rights Reserved.


#include "Settings/DEPRECATED_LimenToggleSetting.h"

#include "BlueprintLibraries/LimenCoreStatics.h"


UDEPRECATED_LimenToggleSetting::UDEPRECATED_LimenToggleSetting() : bDefaultSettingState(false),
											 bCurrentSettingState(false),
											 bPreviousSettingState(false)
{
	PossibleStates.Push(false);
	PossibleStates.Push(true);
}

const TArray<bool>& UDEPRECATED_LimenToggleSetting::GetSettingValues() const
{
	/// I know this is a laughing matter but I guess it's good to keep tradition with the other classes.
	/// Probably no one will ever use this function but here it is just in case.
	return PossibleStates;
}

bool UDEPRECATED_LimenToggleSetting::GetCurrentValue() const
{
	return bCurrentSettingState;
}

bool UDEPRECATED_LimenToggleSetting::GetPreviousValue() const
{
	return bPreviousSettingState;
}

bool UDEPRECATED_LimenToggleSetting::IsValueValid(const bool& Test)
{
	// Parent is pure virtual, do NOT call it!
	return true;
}

bool UDEPRECATED_LimenToggleSetting::CanEdit() const
{
	return true;
}

bool UDEPRECATED_LimenToggleSetting::SetNewValue(const bool& NewSelection)
{
	if (!TLimenEditableSetting::SetNewValue(NewSelection))
	{
		ULimenCoreStatics::LimenLog(this, FString::Printf(TEXT("Error setting '%s' to '%d'"), *GetDisplayName().ToString(), NewSelection), ELogType::Error);
		return false;
	}
 	
	bPreviousSettingState = bCurrentSettingState;
	bCurrentSettingState = NewSelection;
	OnSettingUpdated.Broadcast(this);
	return true;
}

void UDEPRECATED_LimenToggleSetting::SetDefaults()
{
	Super::SetDefaults();

	bPreviousSettingState = bCurrentSettingState;
}

void UDEPRECATED_LimenToggleSetting::SetDefaultValue()
{
	bPreviousSettingState = bCurrentSettingState;
	bCurrentSettingState = bDefaultSettingState;

	Super::SetDefaultValue();
}

void UDEPRECATED_LimenToggleSetting::PostDataLoaded()
{
	bPreviousSettingState = bCurrentSettingState;
	
	Super::PostDataLoaded();
}
