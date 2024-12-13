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
		// No setting was found
		ULimenCoreStatics::LimenLog(this, TEXT("No such setting with dev name '%s' was found"), ELogType::Error);
		return false;
	}

	const FLimenEditableSetting* EditableInterface = reinterpret_cast<FLimenEditableSetting*>(Setting);
	if (EditableInterface == nullptr)
	{
		// No editable interface? No edit!
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

bool ULimenModularSettingsSubsystem::EditValueSetting(const TSubclassOf<ULimenSetting>& Class, const float& NewValue)
{
	return EditSetting<float>(Class, NewValue);
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
