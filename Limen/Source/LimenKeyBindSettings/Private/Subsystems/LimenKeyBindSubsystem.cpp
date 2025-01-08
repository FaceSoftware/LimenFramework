// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenKeyBindSubsystem.h"

#include "Developer/LimenKeyBindDeveloperSettings.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "GameFramework/Pawn.h"


ULimenKeyBindSubsystem::ULimenKeyBindSubsystem()
{
	SaveDataName = TEXT("GameSettings");
}

bool ULimenKeyBindSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	return GetDefault<ULimenKeyBindDeveloperSettings>()->bUseSubsystem;
}

void ULimenKeyBindSubsystem::LoadDefaultSettingsList()
{
	Super::LoadDefaultSettingsList();

	SubsystemSettings = GetDefault<ULimenKeyBindDeveloperSettings>();
	for (const auto& SettingClass : SubsystemSettings->SettingsList)
	{
		if (SettingClass.IsNull())
		{
			// Don't crash the editor if the class is not set
			continue;
		}
		
		ULimenSetting* NewSetting = NewObject<ULimenSetting>(this, SettingClass.LoadSynchronous());
		check(NewSetting != nullptr);
		NewSetting->InitializeSetting();
		AddItem(NewSetting);
	}

	PlayerMappingContexts = SubsystemSettings->PlayerMappingContexts;
	PawnMappingContexts = SubsystemSettings->PawnMappingContexts;
}

UInputMappingContext* ULimenKeyBindSubsystem::GetPlayerInputMappingContext(
	const APlayerController* PlayerController) const
{
	return GetPlayerInputMappingContext(PlayerController->GetClass());
}

UInputMappingContext* ULimenKeyBindSubsystem::GetPawnInputMappingContext(const APawn* Pawn) const
{
	return GetPawnInputMappingContext(Pawn->GetClass());
}

UInputMappingContext* ULimenKeyBindSubsystem::GetPlayerInputMappingContext(
	const TSubclassOf<APlayerController>& PlayerController) const
{
	const FMappingContextSoftPtr* Mapping = PlayerMappingContexts.Find(PlayerController.Get());
	if (Mapping == nullptr)
	{
		return nullptr;
	}

	return Mapping->LoadSynchronous();
}

UInputMappingContext* ULimenKeyBindSubsystem::GetPawnInputMappingContext(const TSubclassOf<APawn>& Pawn) const
{
	const FMappingContextSoftPtr* Mapping = PawnMappingContexts.Find(Pawn.Get());
	if (Mapping == nullptr)
	{
		return nullptr;
	}

	return Mapping->LoadSynchronous();
}
