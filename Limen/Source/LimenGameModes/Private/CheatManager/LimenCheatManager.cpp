// Copyright Face Software. All Rights Reserved.


#include "CheatManager/LimenCheatManager.h"

#include "Actors/LimenGameplayManager.h"
#include "Attributes/LimenHealthAttribute.h"
#include "Interfaces/LimenStartTileInterface.h"
#include "LimenAbilitySystem/Public/Components/LimenAbilityComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "MapBuilders/LimenProceduralTileMapBuilder.h"
#include "Subsystems/LimenGameSaveSubsystem.h"
#include "Tiles/LimenMapTile.h"

class ALimenProceduralTileMapBuilder;

void ULimenCheatManager::ForceCrash()
{
	// Let's definitely crash
	LIMEN_LOG(LogLimen, Warning, this, "Force crash requested!");
	*static_cast<int*>(nullptr) = 0;
	check(false);
}

void ULimenCheatManager::Die()
{
	if (GetPlayerController() == nullptr)
	{
		return;
	}
	
	if (GetPlayerController()->GetPawn() == nullptr)
	{
		return;
	}
	
	ULimenAbilityComponent* AbilityComponent = GetPlayerController()->GetPawn()->GetComponentByClass<ULimenAbilityComponent>();
	if (AbilityComponent == nullptr)
	{
		return;
	}

	ULimenHealthAttribute* Health = AbilityComponent->GetAttribute<ULimenHealthAttribute>();
	if (Health == nullptr)
	{
		return;
	}

	Health->SetCurrentValueAsMin();
}

void ULimenCheatManager::ForceGarbageCollection()
{
	GEngine->ForceGarbageCollection(true);
}

void ULimenCheatManager::SaveGame()
{
	ULimenGameSaveSubsystem* GameSave = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGameSaveSubsystem>();
	GameSave->SaveCurrentGame(GetWorld());
}

void ULimenCheatManager::LoadGame()
{
	ULimenGameSaveSubsystem* GameSave = GetWorld()->GetGameInstance()->GetSubsystem<ULimenGameSaveSubsystem>();
	GameSave->LoadCurrentGame(GetWorld());
}

void ULimenCheatManager::God()
{
	const APawn* PlayerPawn = GetPlayerController()->GetPawn();
	if (PlayerPawn == nullptr)
	{
		return;
	}

	ULimenAbilityComponent* Abilities = PlayerPawn->GetComponentByClass<ULimenAbilityComponent>();
	if (Abilities == nullptr)
	{
		return;
	}

	ULimenHealthAttribute* Health = Abilities->GetAttribute<ULimenHealthAttribute>();
	if (Health == nullptr)
	{
		return;
	}

	if (Health->IsFrozen())
	{
		Health->FreezeAttribute(false);
		LIMEN_LOG(LogLimen, Log, this, "God mode off");
	}
	else
	{
		Health->FreezeAttribute(true);
		LIMEN_LOG(LogLimen, Log, this, "God mode on");
	}
}

void ULimenCheatManager::TakeDamage(const float Damage)
{
	APawn* Pawn = GetPlayerController()->GetPawn();
	if (Pawn == nullptr)
	{
		return;
	}

	ULimenAbilityComponent* AbilityComponent = Pawn->GetComponentByClass<ULimenAbilityComponent>();
	if (AbilityComponent == nullptr)
	{
		return;
	}

	ULimenHealthAttribute* HealthAttribute = AbilityComponent->GetAttribute<ULimenHealthAttribute>();
	if (HealthAttribute == nullptr)
	{
		return;
	}

	const float AbsoluteDamage = FMath::Abs(Damage);
	HealthAttribute->ModifyValueBy(-AbsoluteDamage);
}
