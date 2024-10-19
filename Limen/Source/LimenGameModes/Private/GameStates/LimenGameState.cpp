// Copyright Face Software. All Rights Reserved.


#include "GameStates/LimenGameState.h"

#include "Actors/LimenGameplayManager.h"
#include "Actors/LimenProceduralTileMapBuilder.h"
#include "Net/UnrealNetwork.h"
#include "PlayerStates/LimenPlayerState.h"


ALimenGameState::ALimenGameState(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	LimenPlayerList.Empty();
}

void ALimenGameState::AddPlayerToPlayerList(ALimenPlayerState* InPlayerState)
{
	if (!IsValid(InPlayerState))
	{
		return;
	}
	
	LimenPlayerList.Push(InPlayerState);
}

void ALimenGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALimenGameState, LimenPlayerList)
}

void ALimenGameState::OnRep_LimenPlayerList(const TArray<ALimenPlayerState*>& NewLimenPlayerList)
{
}
