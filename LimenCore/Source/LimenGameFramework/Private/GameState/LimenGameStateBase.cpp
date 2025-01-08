// Copyright Face Software. All Rights Reserved.


#include "GameState/LimenGameStateBase.h"

#include "Net/UnrealNetwork.h"
#include "PlayerState/LimenPlayerStateBase.h"


void ALimenGameStateBase::AddPlayerToPlayerList(ALimenPlayerStateBase* InPlayerState)
{
	if (!IsValid(InPlayerState))
	{
		return;
	}
	
	LimenPlayerList.Push(InPlayerState);
}

void ALimenGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ALimenGameStateBase, LimenPlayerList)
}

void ALimenGameStateBase::OnRep_LimenPlayerList(const TArray<ALimenPlayerStateBase*>& NewLimenPlayerList)
{
}

