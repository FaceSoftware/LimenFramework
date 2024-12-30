// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageItem.h"
#include "LimenSetting.generated.h"

class ULimenModularSettingsSubsystem;
class ULimenRecurrentAction;

/**
 * Abstract base class for a setting.
 * A setting can be anything the user can change like Resolution, WindowMode, etc.
 */
UCLASS(Abstract, NotBlueprintable, BlueprintType)
class LIMENMODULARSETTINGS_API ULimenSetting : public ULimenStorageItem
{
	GENERATED_BODY()
	
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSettingUpdated, const ULimenSetting*, UpdatedSetting);

public:
	UPROPERTY(BlueprintAssignable)
	FSettingUpdated OnSettingUpdated;
	UPROPERTY(BlueprintAssignable)
	FSettingUpdated OnSettingApplied;
	UPROPERTY(BlueprintAssignable)
	FSettingUpdated OnSettingEditableStateChanged;
	
	ULimenSetting();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")
	const FText& GetDescription() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Modular Settings")	
	void ApplySetting();
	
	virtual void SetDefaultValue();

	void InitializeSetting();

	/**
	 * @brief Function executed when the subsystem has initialized all settings.
	 * Useful for binding events when this setting depends on another.
	 * This should be only called by the subsystem.
	 * @param ModularSettingsSubsystem The subsystem that initialized this setting.
	 */
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem);
	
protected:
	UFUNCTION()
	virtual void RecurrentAction();
	UFUNCTION()
	virtual bool ShouldStopRecurrentAction();
	UFUNCTION()
	virtual void ActionSuccessful();
	UFUNCTION()
	virtual void ActionUnsuccessful();
	
	template<typename SubsystemClass>
	SubsystemClass* GetOwnerSubsystem() const
	{
		SubsystemClass* Subsystem = Cast<SubsystemClass>(OwnerSubsystem.Get());
		check(Subsystem != nullptr);
		return Subsystem;
	}
	
	/**
	 * @brief Sets default variable values of the setting, such as possible states, default state, etc.
	 * Can also serve for initialization purposes.
	 * @warning Must be overriden
	 */
	virtual void SetDefaults();
	/**
	 * @brief (For blueprints) Sets default variable values of the setting, such as possible states, default state, etc.
	 * Can also serve for initialization purposes.
	 * @warning Must be overriden
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Modular Settings", DisplayName="Set Defaults")
	void BP_SetDefaults();
	/**
	 * @brief Should apply the current setting without notifying the OnSettingChanged delegate
	 * @warning Must be overriden
	 */
	virtual void ApplyCurrentSetting();
	
	virtual void DataLoaded() override;
	
	UPROPERTY(EditAnywhere, Category="Limen")
	FText Description;

	UPROPERTY()
	TObjectPtr<ULimenModularSettingsSubsystem> OwnerSubsystem;

	bool bUseRecurrentAction;

private:
	UPROPERTY()
	TObjectPtr<ULimenRecurrentAction> RecurrentActionObject;

	bool bHasInitialized;

	void DestroyRecurrentActionObject();
};



/**
 * @brief Interface for readable settings. Not intended to be directly inherited, see TLimenReadableSetting for that
 */
class FLimenEditableSetting
{
public:
	
	virtual ~FLimenEditableSetting() = default;

	/**
	 * @brief Defines the rules of editing this setting.
	 * Egg: Is the setting supported by the system? Can the setting be changed in the current game state? etc.
	 * @return True if it can be edited, false if not.
	 */
	virtual bool CanEdit() const = 0;
};

/**
 * @brief Interface for editable settings. Not intended to be directly inherited, see TLimenEditableSetting for that
 */
class FLimenReadableSetting
{
public:
	virtual ~FLimenReadableSetting() = default;
};

/**
 * @brief Template interface for editable settings
 * @tparam SettingType - The settings variable type
 */
template<typename SettingType>
class TLimenEditableSetting : public FLimenEditableSetting
{
public:
	
	virtual ~TLimenEditableSetting() override = default;

	/**
	 * @brief Setter for the new valid. Returns a bool because it performs validation with CanEdit() and IsSelectionValid().
	 * The function will fail if one of these also fails.
	 * @param NewValue The new value to set to.
	 * @return Whether or not the value was successfully set.
	 */
	virtual bool SetNewValue(const SettingType& NewValue)
	{
		if (!CanEdit() || !IsValueValid(NewValue))
		{
			return false;
		}
		
		return true;
	}
	
protected:
	/**
	 * @brief Defines the rules for changing into a new selection. Does it exists? Is it valid? etc.
	 * @param Test The value to test against.
	 * @return True if it is valid, false if not.
	 */
	virtual bool IsValueValid(const SettingType& Test) = 0;
};

/**
 * @brief Interface for readable settings
 * @tparam SettingType - The settings variable type
 */
template<typename SettingType>
class TLimenReadableSetting : public FLimenReadableSetting
{
public:
	/**
	 * @brief Getter for the available states
	 * @return The available states
	 */
	virtual const TArray<SettingType>& GetSettingValues() const = 0;

	/**
	 * @brief Getter for the current selection
	 * @return The current selection
	 */
	virtual SettingType GetCurrentValue() const = 0;

	virtual SettingType GetPreviousValue() const = 0;
};
