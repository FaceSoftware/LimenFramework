// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenModularSettingsSubsystem.h"

#include "Engine/Engine.h"


void ULimenModularSettingsSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	// Load default values
	LoadDefaultSettingsList();

	for (auto* Setting : GetItems<ULimenSetting>())
	{
		check(Setting != nullptr);
		Setting->OnSettingUpdated.AddUniqueDynamic(this, &ThisClass::OnSettingUpdated);
		Setting->SubsystemInitialized(this);
	}
	FWorldDelegates::OnPostWorldInitialization.AddUObject(this, &ThisClass::PostWorldInitialization);
	
	Super::Initialize(Collection);
}

void ULimenModularSettingsSubsystem::Deinitialize()
{
	FWorldDelegates::OnPostWorldInitialization.RemoveAll(this);
	Save();
	
	Super::Deinitialize();
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

bool ULimenModularSettingsSubsystem::ShouldSaveData() const
{
	return true;
}

bool ULimenModularSettingsSubsystem::ShouldLoadData() const
{
	return true;
}

void ULimenModularSettingsSubsystem::DataLoaded()
{
}

void ULimenModularSettingsSubsystem::DataSaved()
{
}

void ULimenModularSettingsSubsystem::LoadDefaultSettingsList()
{
}

void ULimenModularSettingsSubsystem::OnSettingUpdated(const ULimenSetting* UpdatedSetting)
{
	Save();
	GEngine->ForceGarbageCollection(true);
}

void ULimenModularSettingsSubsystem::PostWorldInitialization(UWorld* World,
	const UWorld::InitializationValues InitValues)
{
	World->OnWorldBeginPlay.AddLambda([this]
	{
		Load();
	});
}
