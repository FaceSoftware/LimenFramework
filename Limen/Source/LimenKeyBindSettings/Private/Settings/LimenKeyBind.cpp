// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Settings/LimenKeyBind.h"

#include "InputMappingContext.h"
#include "Developer/LimenKeyBindDeveloperSettings.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
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
		FName KeyName;
		Ar << KeyName;
		Key = FKey(KeyName);
	}
	
	return true;
}

ULimenKeyBind::ULimenKeyBind()
{
	DevelopmentName = TEXT("");
	Category = FText::FromString(TEXT(""));
	DisplayName = FText::FromString(TEXT(""));
	Description = FText::FromString(TEXT(""));
	bCanEdit = true;
}

void ULimenKeyBind::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);

	CurrentKeyMapping.Serialize(Ar);
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
	return Test.Action == InputAction && Test.Key != EKeys::Invalid;
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
	
	ULimenKeyBindSubsystem* KeyBindSubsystem = GetWorld()->GetGameInstance()->GetSubsystem<ULimenKeyBindSubsystem>();
	verify(KeyBindSubsystem->SetActionKeyMappingByAction(InputAction.Get(), CurrentKeyMapping));

	OnSettingApplied.Broadcast(this);
}

void ULimenKeyBind::SetDefaults()
{
	Super::SetDefaults();

	if (InputAction == nullptr)
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
			if (Mapping.Action != InputAction)
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
	
	Super::SetDefaultValue();
}

void ULimenKeyBind::DataLoaded()
{
	PreviousKeyMapping = CurrentKeyMapping;
	
	Super::DataLoaded();
}
