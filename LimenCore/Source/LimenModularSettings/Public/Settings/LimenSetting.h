// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenStorageItem.h"
#include "HAL/IConsoleManager.h"
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
	static const EConsoleVariableFlags ConsoleVariableUserSettingFlag; 
	
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
	void ApplySetting(bool bUserRequest = false);
	
	virtual void SetDefaultValue();

	void InitializeSetting(ULimenModularSettingsSubsystem* InOwnerSubsystem);

	/**
	 * @brief Function executed when the subsystem has initialized all settings.
	 * Useful for binding events when this setting depends on another.
	 * This should be only called by the subsystem.
	 * @param ModularSettingsSubsystem The subsystem that initialized this setting.
	 */
	virtual void SubsystemInitialized(ULimenModularSettingsSubsystem* ModularSettingsSubsystem);
	
protected:

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
	 * @warning Should be overriden
	 */
	virtual void SetDefaults();
	/**
	 * @brief (For blueprints) Sets default variable values of the setting, such as possible states, default state, etc.
	 * Can also serve for initialization purposes.
	 * @warning Should be overriden
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Modular Settings", DisplayName="Set Defaults")
	void BP_SetDefaults();
	/**
	 * @brief Should apply the current setting without notifying the OnSettingChanged delegate.
	 * @param bUserRequest Whether the setting change was manually requested by the user through UI or an internal system.
	 * @warning Should be overriden
	 */
	virtual void ApplyCurrentSetting(bool bUserRequest = false);
	
	virtual void DataLoaded() override;
	virtual bool ShouldLoadData() const override;
	
	UPROPERTY(EditAnywhere, Category="Limen")
	FText Description;

	UPROPERTY()
	TObjectPtr<ULimenModularSettingsSubsystem> OwnerSubsystem;

private:
	bool bHasInitialized;
	bool bShouldLoadData;
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

struct FConsoleSetting
{
	FConsoleSetting() = default;
	FConsoleSetting(const FConsoleSetting& Other) = default;
	FConsoleSetting(FConsoleSetting&& Other) = default;
	virtual ~FConsoleSetting() = default;

	virtual FString GetName() const = 0;
    virtual void SetName(const FString& InName) = 0;

	virtual void ApplyCVars() = 0;
	virtual void ResetCVars() = 0;
};

typedef FConsoleSetting* FConsoleSettingPtr;

template<typename SettingValue>
struct TConsoleSetting final : FConsoleSetting
{
	TConsoleSetting() = default;
	explicit TConsoleSetting(const FString& InName) : FConsoleSetting(), Name(InName)
	{
	}

	virtual FString GetName() const override { return Name; }
	virtual void SetName(const FString& InName) override { Name = InName; }

	void AddCVar(const FString& CVarName, const SettingValue CVarValue)
	{
		CVars.Add(CVarName, CVarValue);
	}

	virtual void ApplyCVars() override
	{
		if (CVars.IsEmpty()) return;
		for (auto& Variable : CVars)
		{
			FString& VarName = Variable.Key;
			SettingValue& VarValue = Variable.Value;

			IConsoleVariable* Var = IConsoleManager::Get().FindConsoleVariable(*VarName);
			check(Var != nullptr);
			Var->Set(VarValue, ULimenSetting::ConsoleVariableUserSettingFlag);
		}
	}

	virtual void ResetCVars() override
	{
		CVars.Empty();
	}

private:
	FString Name;
	TMap<FString, SettingValue> CVars;
};
