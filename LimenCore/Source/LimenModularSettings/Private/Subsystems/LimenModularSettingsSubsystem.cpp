// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenModularSettingsSubsystem.h"


void ULimenModularSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Load default values
	LoadDefaultSettingsList();

	for (auto* Setting : GetItems<ULimenSetting>())
	{
		check(Setting != nullptr);
		Setting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::SettingUpdated);
		Setting->SubsystemInitialized(this);
	}
	
	Super::Initialize(Collection);
}

void ULimenModularSettingsSubsystem::Deinitialize()
{
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	Save();
	
	Super::Deinitialize();
}

bool ULimenModularSettingsSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer)) { return false; }
	return !IsRunningDedicatedServer();
}

void ULimenModularSettingsSubsystem::PlayerControllerChanged(APlayerController* NewPlayerController)
{
	CurrentPlayerController = NewPlayerController;
	if (!CurrentPlayerController.IsValid()) { return; }
	
	for (ULimenSetting* const& Setting : GetItems<ULimenSetting>())
	{
		Setting->ApplySetting(false);
	}
}

bool ULimenModularSettingsSubsystem::CanEditSetting(const TSubclassOf<ULimenSetting>& Class) const
{
	ULimenSetting* Setting = GetItem<ULimenSetting>(Class);
	if (Setting == nullptr)
	{
		return false;
	}

	const FLimenEditableSetting* EditableInterface = reinterpret_cast<FLimenEditableSetting*>(Setting);
	if (EditableInterface == nullptr)
	{
		return false;
	}

	return EditableInterface->CanEdit();
}

bool ULimenModularSettingsSubsystem::EditSelectionSetting(const TSubclassOf<ULimenSetting>& Class, const FString& NewSelection)
{
	return EditSetting<FString>(Class, NewSelection);
}

bool ULimenModularSettingsSubsystem::EditToggleSetting(const TSubclassOf<ULimenSetting>& Class, const bool NewState)
{
	return EditSetting<bool>(Class, NewState);
}

bool ULimenModularSettingsSubsystem::EditValueSetting(const TSubclassOf<ULimenSetting>& Class, const float NewValue)
{
	return EditSetting<float>(Class, NewValue);
}

void ULimenModularSettingsSubsystem::ResetAllToDefault()
{
	for (auto*& Setting : GetItems<ULimenSetting>())
	{
		Setting->SetDefaultValue();
		Setting->ApplySetting();
	}
}

void ULimenModularSettingsSubsystem::ResetSettingToDefault(const TSubclassOf<ULimenSetting>& Class)
{
	auto* const& Setting = GetItem<ULimenSetting>(Class);
	Setting->SetDefaultValue();
	Setting->ApplySetting();
}

ULimenSetting* ULimenModularSettingsSubsystem::GetSetting(const TSubclassOf<ULimenSetting> Class) const
{
	return GetItem<ULimenSetting>(Class);
}

void ULimenModularSettingsSubsystem::LoadDefaultSettingsList()
{
}

void ULimenModularSettingsSubsystem::SettingUpdated(const ULimenSetting* UpdatedSetting)
{
	Save();
	OnSettingUpdated.Broadcast(UpdatedSetting);
}
