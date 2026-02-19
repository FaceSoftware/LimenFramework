// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "Engine/World.h"
#include "LimenStorageSubsystem.h"
#include "Settings/LimenSetting.h"
#include "LimenModularSettingsSubsystem.generated.h"


class ULimenModularSettingsSubsystemDeveloperSettings;
class ULimenSettingsSaveData;

/**
 * Base class for settings subsystems. A settings subsystem is essentially a manager for a specific set of settings.
 * It handles things like saving them, their lifetime, and it should act as a wrapper for the settings.
 * It should not contain any specific information regarding a setting. 
 */
UCLASS(Abstract, BlueprintType)
class LIMENMODULARSETTINGS_API ULimenModularSettingsSubsystem : public ULimenStorageSubsystem, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FSettingUpdated OnSettingUpdated; 

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	bool CanEditSetting(const TSubclassOf<ULimenSetting>& Class) const;

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	bool EditSelectionSetting(const TSubclassOf<ULimenSetting>& Class, const FString& NewSelection);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	bool EditToggleSetting(const TSubclassOf<ULimenSetting>& Class, const bool NewState);

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	bool EditValueSetting(const TSubclassOf<ULimenSetting>& Class, const float NewValue);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	void ResetAllToDefault();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	void ResetSettingToDefault(const TSubclassOf<ULimenSetting>& Class);

	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings", meta=(DeterminesOutputType=Class))
	ULimenSetting* GetSetting(const TSubclassOf<ULimenSetting> Class) const;

	template<typename SettingType>
	bool EditSetting(const TSubclassOf<ULimenSetting>& Class, const SettingType& NewValue);

	template<typename SettingType>
	bool EditSetting(ULimenSetting* InSetting, const SettingType& NewValue);
	
	template<typename SettingType>
	bool ReapplySetting(const TSubclassOf<ULimenSetting>& Class);
	
	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;
	virtual FName GetUniqueDeterministicId() const override;
	
protected:

	/**
	 * @brief Used to load the settings list into the 'Settings' variable.
	 * Needs to be implemented in a child class.
	 * It is recommended to load from a custom ULimenModularSettingsSubsystemDeveloperSettings-derived class but not mandatory.
	 * Called before loading the settings so we know which settings still exist.
	 * @warning Must be overriden
	 */
	virtual void LoadDefaultSettingsList();

	UFUNCTION()
	virtual void SettingUpdated(const ULimenSetting* UpdatedSetting);
	
	virtual void WorldInitializedActors(const FActorsInitializedParams& InitParams);
	
private:

};

template <typename SettingType>
bool ULimenModularSettingsSubsystem::EditSetting(const TSubclassOf<ULimenSetting>& Class, const SettingType& NewValue)
{
	ULimenSetting* Setting = GetItem<ULimenSetting>(Class);
	if (Setting == nullptr)
	{
		return false;
	}
		
	return EditSetting(Setting, NewValue);
}

template <typename SettingType>
bool ULimenModularSettingsSubsystem::EditSetting(ULimenSetting* InSetting, const SettingType& NewValue)
{
	if (InSetting == nullptr)
	{
		return false;
	}
		
	TLimenEditableSetting<SettingType>* EditableInterface = reinterpret_cast<TLimenEditableSetting<SettingType>*>(InSetting);
	if (EditableInterface == nullptr)
	{
		return false;
	}
		
	EditableInterface->SetNewValue(NewValue);
	return true;
}

template <typename SettingType>
bool ULimenModularSettingsSubsystem::ReapplySetting(const TSubclassOf<ULimenSetting>& Class)
{
	ULimenSetting* Setting = GetItem<ULimenSetting>(Class);
	if (Setting == nullptr)
	{
		return false;
	}

	TLimenEditableSetting<SettingType>* EditableInterface = reinterpret_cast<TLimenEditableSetting<SettingType>*>(Setting);
	if (EditableInterface == nullptr)
	{
		return false;
	}
	TLimenReadableSetting<SettingType>* ReadableInterface = reinterpret_cast<TLimenReadableSetting<SettingType>*>(Setting);
	if (ReadableInterface == nullptr)
	{
		return false;
	}

	const SettingType& CurrentValue = ReadableInterface->GetCurrentValue();
	EditableInterface->SetNewValue(CurrentValue);
	return true;
}
