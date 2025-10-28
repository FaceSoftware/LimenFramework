// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayActor.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utils/LimenReplicationUtils.h"


ALimenGameplayActor::ALimenGameplayActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GameplayState = ELimenGameplayActorState::Undefined;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ALimenGameplayActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params(COND_None, REPNOTIFY_OnChanged, true);

	DOREPLIFETIME_WITH_PARAMS_FAST(ALimenGameplayActor, GameplayState, Params)
}

void ALimenGameplayActor::RemoveFromGameplay()
{
	if (SHOULD_PREDICT_NETWORK_EVENT)
	{
		RemoveFromGameplayInternal();
	}

	Server_RemoveFromGameplay();
}

void ALimenGameplayActor::AddToGameplay(const bool bEnableCollision)
{
	if (SHOULD_PREDICT_NETWORK_EVENT)
	{
		AddToGameplayInternal(bEnableCollision);
	}

	Server_AddToGameplay(bEnableCollision);
}

bool ALimenGameplayActor::IsRemovedFromGameplay() const
{
	return GameplayState == ELimenGameplayActorState::OutOfGameplay;
}

void ALimenGameplayActor::OnRep_GameplayState()
{
	GameplayStateChanged();
}

void ALimenGameplayActor::RemoveFromGameplayInternal()
{
	if (IsRemovedFromGameplay()) { return; }

	GameplayState = ELimenGameplayActorState::OutOfGameplay;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GameplayState, this);
	
	GameplayStateChanged();
}

void ALimenGameplayActor::AddToGameplayInternal(const bool bEnableCollision)
{
	if (bEnableCollision && GameplayState == ELimenGameplayActorState::InGameplayCollisionEnabled) { return; }
	if (!bEnableCollision && GameplayState == ELimenGameplayActorState::InGameplayCollisionDisabled) { return; }

	GameplayState = bEnableCollision ? ELimenGameplayActorState::InGameplayCollisionEnabled
									 : ELimenGameplayActorState::InGameplayCollisionDisabled;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GameplayState, this);

	GameplayStateChanged();
}

void ALimenGameplayActor::GameplayStateChanged()
{
	switch (GameplayState)
	{
	case ELimenGameplayActorState::InGameplayCollisionEnabled:
		{
			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);	
		}
		break;
	case ELimenGameplayActorState::InGameplayCollisionDisabled:
		{
			SetActorHiddenInGame(false);
			SetActorEnableCollision(false);
		}
		break;
	case ELimenGameplayActorState::OutOfGameplay:
		{
			SetActorHiddenInGame(true);
			SetActorEnableCollision(false);
		}
		break;
	default:
		break;
	}
}

void ALimenGameplayActor::Server_RemoveFromGameplay_Implementation()
{
	RemoveFromGameplayInternal();
}

void ALimenGameplayActor::Server_AddToGameplay_Implementation(const bool bEnableCollision)
{
	AddToGameplayInternal(bEnableCollision);
}
