// Copyright Face Software. All Rights Reserved.


#include "CheatManager/LimenCheatManager.h"

#include "Actors/LimenGameplayManager.h"
#include "Actors/LimenMapTile.h"
#include "Actors/LimenProceduralTileMapBuilder.h"
#include "Items/LimenItemBase.h"
#include "Attributes/LimenHealthAttribute.h"
#include "..\..\..\LimenPlayers\Public\Characters\LimenCharacterBase.h"
#include "Components/Interaction/LimenInteractionComponent.h"
#include "Interfaces/LimenStartTileInterface.h"
#include "LimenAbilitySystem/Public/Components/LimenAbilityComponent.h"
#include "LogMacros/LimenLogMacros.h"
#include "Subsystems/LimenGameSaveSubsystem.h"

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

void ULimenCheatManager::LoadTileMap(const int TileMapIndex)
{
	auto* MapBuilder = ALimenGameplayManager::GetGameplayManager<ALimenProceduralTileMapBuilder>(GetWorld());
	if (!IsValid(MapBuilder))
	{
		return;
	}

	MapBuilder->UnloadMap();
	if (!MapBuilder->LoadMapWithIndex(TileMapIndex))
	{
		return;
	}
	
	MapBuilder->OnBuildProcessFinished.AddUniqueDynamic(this, &ThisClass::ULimenCheatManager::OnTileMapLoaded);
	MapBuilder->StartBuildProcess();
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

void ULimenCheatManager::TeleportToEndTile()
{
	ALimenProceduralTileMapBuilder* TileMapBuilder = ALimenGameplayManager::GetGameplayManager<ALimenProceduralTileMapBuilder>(this);
	if (TileMapBuilder == nullptr)
	{
		LIMEN_LOG(LogLimen, Error, this, "Could not teleport: Unable to find a tile map builder");
		return;
	}

	const ALimenMapTile* EndMapTile = Cast<ALimenMapTile>(TileMapBuilder->GetEndTile().GetObject());
	if (EndMapTile == nullptr)
	{
		LIMEN_LOG(LogLimen, Error, this, "Could not teleport: Unable to find end tile (make sure it implements the appropriate interface)");
		return;
	}

	APawn* PlayerPawn = GetPlayerController()->GetPawnOrSpectator();
	if (PlayerPawn == nullptr)
	{
		LIMEN_LOG(LogLimen, Error, this, "Could not teleport: Unable to find player pawn");
		return;
	}

	FVector Origin, Extent;
	EndMapTile->GetActorBounds(true, Origin, Extent, false);
	PlayerPawn->SetActorLocation(Origin);
		LIMEN_LOG(LogLimen, Log, this, "Teleported %s to end tile!", *PlayerPawn->GetHumanReadableName());
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

void ULimenCheatManager::OnTileMapLoaded()
{
	auto* MapBuilder = ALimenGameplayManager::GetGameplayManager<ALimenProceduralTileMapBuilder>(GetWorld());
	if (!IsValid(MapBuilder))
	{
		return;
	}

	MapBuilder->DestroyPreviousMap();
	
	const ALimenMapTile* Tile = Cast<ALimenMapTile>(MapBuilder->GetStartTile().GetObject());
	if (Tile == nullptr)
	{
		return;
	}

	check(Tile->Implements<ULimenStartTileInterface>());
	const FVector SpawnLocation = ILimenStartTileInterface::Execute_GetDefaultSpawnLocation(Tile);
	GetPlayerController()->GetPawn()->SetActorLocation(SpawnLocation);
}
