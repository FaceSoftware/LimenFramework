// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Subsystems/LimenKeyBindSubsystem.h"

#include "Developer/LimenKeyBindDeveloperSettings.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h"
#include "PlayerMappableKeySettings.h"
#include "GameFramework/Pawn.h"
#include "Settings/LimenKeyBind.h"


ULimenKeyBindSubsystem::ULimenKeyBindSubsystem()
{
	SaveDataName = TEXT("InputSettings");
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
	
	for (const auto& SettingClass : SubsystemSettings->SettingsList)
	{
		if (SettingClass.IsNull())
		{
			// Don't crash the editor if the class is not set
			continue;
		}
		
		ULimenSetting* NewSetting = NewObject<ULimenSetting>(this, SettingClass.LoadSynchronous());
		
		NewSetting->InitializeSetting();
		AddItem(NewSetting);
	}

	for (const auto& MappingContext : MappingContexts)
	{
		for (int i = 0; i < MappingContext->GetMappings().Num(); ++i)
		{
			FEnhancedActionKeyMapping& MappingRef = MappingContext->GetMapping(i);
			if (!MappingRef.IsPlayerMappable())
			{
				continue;
			}
			
			ULimenKeyBind* KeyBindSetting = NewObject<ULimenKeyBind>(this);
			KeyBindSetting->InitializeSetting(&MappingRef);
			KeyBindSetting->OnSettingApplied.AddUniqueDynamic(this, &ThisClass::SettingApplied);
			AddItem(KeyBindSetting);
		}
	}
}

void ULimenKeyBindSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	SubsystemSettings = GetDefault<ULimenKeyBindDeveloperSettings>();
	
	PlayerMappingContexts.Reserve(SubsystemSettings->PlayerMappingContexts.Num());
	PawnMappingContexts.Reserve(SubsystemSettings->PawnMappingContexts.Num());

	MappingContexts.Reserve(PlayerMappingContexts.Num() + PawnMappingContexts.Num());
	
	for (auto& PlayerMappingContext : SubsystemSettings->PlayerMappingContexts)
	{
		auto PlayerController = PlayerMappingContext.Key;
		auto* MappingContext = PlayerMappingContext.Value.LoadSynchronous();
		
		auto* MappingContextObject = NewObject<UInputMappingContext>(this, MappingContext->GetClass(), NAME_None, RF_NoFlags, MappingContext);
		TStrongObjectPtr MappingContextPtr(MappingContextObject);
		
		PlayerMappingContexts.Add(PlayerController, MappingContextPtr.Get());
		MappingContexts.Push(MappingContextPtr);
	}

	for (auto& PawnMappingContext : SubsystemSettings->PawnMappingContexts)
	{
		auto Pawn = PawnMappingContext.Key;
		auto* MappingContext = PawnMappingContext.Value.LoadSynchronous();
		
		auto* MappingContextObject = NewObject<UInputMappingContext>(this, MappingContext->GetClass(), NAME_None, RF_NoFlags, MappingContext);
		TStrongObjectPtr MappingContextPtr(MappingContextObject);
		
		PawnMappingContexts.Add(Pawn, MappingContextPtr.Get());
		MappingContexts.Push(TStrongObjectPtr(MappingContextPtr));
	}
	
	Super::Initialize(Collection);
}

void ULimenKeyBindSubsystem::Deinitialize()
{
	Super::Deinitialize();

	MappingContexts.Empty();
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
	UInputMappingContext* const* Mapping = PlayerMappingContexts.Find(PlayerController.Get());
	if (Mapping == nullptr)
	{
		return nullptr;
	}

	return *Mapping;
}

UInputMappingContext* ULimenKeyBindSubsystem::GetPawnInputMappingContext(const TSubclassOf<APawn>& Pawn) const
{
	UInputMappingContext* const* Mapping = PawnMappingContexts.Find(Pawn.Get());
	if (Mapping == nullptr)
	{
		return nullptr;
	}

	return *Mapping;
}

UInputMappingContext* ULimenKeyBindSubsystem::GetInputMappingByAction(const UInputAction* Action) const
{
	auto* Context = MappingContexts.FindByPredicate([this, &Action] (const TStrongObjectPtr<UInputMappingContext>& MappingContext)
	{
		return MappingContext->GetMappings().FindByPredicate([this, &Action] (const FEnhancedActionKeyMapping& KeyMapping)
		{
			return KeyMapping.Action == Action;
		}) != nullptr;
	});

	if (Context == nullptr)
	{
		return nullptr;
	}

	return Context->Get();
}

FEnhancedActionKeyMapping& ULimenKeyBindSubsystem::GetActionKeyMappingByAction(const UInputAction* Action)
{
	int32 Index = INDEX_NONE;
	UInputMappingContext* MappingContextPtr = nullptr;
	
	for (auto& MappingContext : MappingContexts)
	{
		Index = MappingContext->GetMappings().IndexOfByPredicate([this, &Action] (const FEnhancedActionKeyMapping& KeyMapping)
		{
			return KeyMapping.Action == Action;
		});

		if (Index != INDEX_NONE)
		{
			MappingContextPtr = MappingContext.Get();
			break;
		}
	}

	return MappingContextPtr->GetMapping(Index);
}

bool ULimenKeyBindSubsystem::SetActionKeyMappingByAction(const UInputAction* Action,
	const FEnhancedActionKeyMapping& InActionKeyMapping)
{
	for (const auto& MappingContext : MappingContexts)
	{
		const int32 Index = MappingContext->GetMappings().IndexOfByPredicate(
			[this, &Action](const FEnhancedActionKeyMapping& KeyMapping)
		{
			return KeyMapping.Action == Action;
		});

		if (Index != INDEX_NONE)
		{
			FEnhancedActionKeyMapping& KeyMappingRef = MappingContext->GetMapping(Index);
			KeyMappingRef = InActionKeyMapping;
			return true;
		}
	}
	
	return false;
}

void ULimenKeyBindSubsystem::SettingApplied(const ULimenSetting* Setting)
{
	const FEnhancedActionKeyMapping KeyMapping = CastChecked<ULimenKeyBind>(Setting)->GetCurrentValue();
	check(KeyMapping.Action != nullptr);
	OnKeyBindUpdate.Broadcast(KeyMapping);
}
