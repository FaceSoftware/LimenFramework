// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenKeyBind.h"

#include "InputMappingContext.h"
#include "Developer/LimenKeyBindDeveloperSettings.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "MappableKeySettings/LimenPlayerMappableKeySettings.h"
#include "Subsystems/LimenKeyBindSubsystem.h"
#include "Subsystems/LimenSaveSubsystem.h"


FSaveableEnhancedActionKeyMapping::FSaveableEnhancedActionKeyMapping(const FEnhancedActionKeyMapping& InKeyMapping)
	: Super(InKeyMapping)
{
}

bool FSaveableEnhancedActionKeyMapping::Serialize(FArchive& Ar)
{
	if (Ar.IsSaving())
	{
		FName TempValue = Key.GetFName();
		Ar << TempValue;

		TempValue = FLimenSerialization::BoolToName(bShouldBeIgnored);
		Ar << TempValue;
		
		uint8 ByteValue = static_cast<uint8>(SettingBehavior);
		Ar << ByteValue;
	}
	else if (Ar.IsLoading())
	{
		FName TempName;
		Ar << TempName;
		Key = FKey(TempName);
		
		Ar << TempName;
		bShouldBeIgnored = FLimenSerialization::NameToBool(TempName);
		
		uint8 ByteValue;
		Ar << ByteValue;
		SettingBehavior = static_cast<EPlayerMappableKeySettingBehaviors>(ByteValue);
	}
	
	return true;
}

ULimenKeyBind::ULimenKeyBind()
{
	DevelopmentName = TEXT("");
	Category = FText::FromString(TEXT("KeybindCategory"));
	DisplayName = FText::FromString(TEXT("KeybindSetting"));
	Description = FText::FromString(TEXT("This is a keybind setting."));
	bCanEdit = false;
	ActionKeyMappingPtr = nullptr;
}

void ULimenKeyBind::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	CurrentKeyMapping.Serialize(Ar);
}

void ULimenKeyBind::InitializeSetting(ULimenModularSettingsSubsystem* InOwnerSubsystem, FEnhancedActionKeyMapping* InActionKeyMapping)
{
	check(InActionKeyMapping->IsPlayerMappable())
	
	const auto* MappableKeySettings = InActionKeyMapping->GetPlayerMappableKeySettings<ULimenPlayerMappableKeySettings>();
	
	DevelopmentName = InActionKeyMapping->GetMappingName();
	DisplayName = InActionKeyMapping->GetDisplayName();
	Category = InActionKeyMapping->GetDisplayCategory();
	Description = MappableKeySettings != nullptr ? MappableKeySettings->Description : FText::FromString(TEXT(""));
	bCanEdit = true;

	ActionKeyMappingPtr = InActionKeyMapping;
	DefaultSelection = *InActionKeyMapping;
	
	Super::InitializeSetting(InOwnerSubsystem);
}

const TArray<FEnhancedActionKeyMapping>& ULimenKeyBind::GetSettingValues() const
{
	return PossibleValues;
}

FEnhancedActionKeyMapping ULimenKeyBind::GetCurrentValue() const
{
	return CurrentKeyMapping;
}

FEnhancedActionKeyMapping ULimenKeyBind::GetPreviousValue() const
{
	return PreviousKeyMapping;
}

bool ULimenKeyBind::IsValueValid(const FEnhancedActionKeyMapping& Test)
{
	return Test.Key != EKeys::Invalid;
}

bool ULimenKeyBind::CanEdit() const
{
	return bCanEdit;
}

bool ULimenKeyBind::SetNewValue(const FEnhancedActionKeyMapping& NewSelection)
{
	if (!TLimenEditableSetting<FEnhancedActionKeyMapping>::SetNewValue(NewSelection))
	{ 
		return false;
	}

	PreviousKeyMapping = CurrentKeyMapping;
	CurrentKeyMapping = FSaveableEnhancedActionKeyMapping(NewSelection);
	OnSettingUpdated.Broadcast(this);
	return true;
}

void ULimenKeyBind::ApplyCurrentSetting(bool bUserRequest)
{
	Super::ApplyCurrentSetting(bUserRequest);
	
	*ActionKeyMappingPtr = CurrentKeyMapping;
	OnSettingApplied.Broadcast(this);
}

void ULimenKeyBind::SetDefaults()
{
	Super::SetDefaults();

	if (DefaultSelection.Action == nullptr)
	{
		return;
	}
	
	for (auto& InputMappingContextSoftPtr : GetDefault<ULimenKeyBindDeveloperSettings>()->GetAllInputMappingContexts())
	{
		if (InputMappingContextSoftPtr.IsNull())
		{
			continue;
		}

		for (const TStrongObjectPtr MappingContextPtr(InputMappingContextSoftPtr.LoadSynchronous()); auto& Mapping : MappingContextPtr->GetMappings())
		{
			if (Mapping.Action != DefaultSelection.Action)
			{
				continue;
			}

			DefaultSelection = Mapping;
			return;
		}
	}

	ensureAlwaysMsgf(false, TEXT("Unbound input action!"));
}

void ULimenKeyBind::SetDefaultValue()
{
	PreviousKeyMapping = CurrentKeyMapping;
	CurrentKeyMapping = FSaveableEnhancedActionKeyMapping(DefaultSelection);
	ULimenPlayerMappableKeySettings* Settings = CurrentKeyMapping.GetPlayerMappableKeySettings<ULimenPlayerMappableKeySettings>();
	if (Settings != nullptr)
	{
		SetInputActionHandlingType(Settings->InputHandlingType);
	}
	
	Super::SetDefaultValue();
}

void ULimenKeyBind::DataLoaded()
{
	PreviousKeyMapping = CurrentKeyMapping;
	
	Super::DataLoaded();
}

void ULimenKeyBind::SetInputActionHandlingType(const EInputActionHandlingType NewType)
{
	ULimenPlayerMappableKeySettings* Settings = CurrentKeyMapping.GetPlayerMappableKeySettings<ULimenPlayerMappableKeySettings>();
	if (Settings == nullptr || Settings->InputHandlingType == NewType)
	{
		return;
	}
	
	Settings->InputHandlingType = NewType;
}
