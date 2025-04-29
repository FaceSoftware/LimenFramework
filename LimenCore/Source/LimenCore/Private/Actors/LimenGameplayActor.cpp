// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayActor.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"


ALimenGameplayActor::ALimenGameplayActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GameplayState = ELimenGameplayActorState::OutOfGameplay;
}

void ALimenGameplayActor:: BeginPlay()
{
	Super::BeginPlay();
}

void ALimenGameplayActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
	Params.Condition = ELifetimeCondition::COND_None;
	Params.RepNotifyCondition = ELifetimeRepNotifyCondition::REPNOTIFY_OnChanged;

	DOREPLIFETIME_WITH_PARAMS_FAST(ALimenGameplayActor, GameplayState, Params);
}

void ALimenGameplayActor::RemoveFromGameplay()
{
	check(HasAuthority())
	if (IsRemovedFromGameplay())
	{
		return;
	}
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	GameplayState = ELimenGameplayActorState::OutOfGameplay;
}

void ALimenGameplayActor::AddToGameplay(const bool bEnableCollision)
{
	check(HasAuthority())
	if (bEnableCollision && GameplayState == ELimenGameplayActorState::InGameplay ||
	   !bEnableCollision && GameplayState == ELimenGameplayActorState::InGameplayWithCollisionDisabled)
	{
		return;
	}
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(bEnableCollision);
	GameplayState = bEnableCollision ? ELimenGameplayActorState::InGameplay : ELimenGameplayActorState::InGameplayWithCollisionDisabled;
}

bool ALimenGameplayActor::IsRemovedFromGameplay() const
{
	return GameplayState == ELimenGameplayActorState::OutOfGameplay;
}

void ALimenGameplayActor::OnRep_GameplayState()
{
	switch (GameplayState)
	{
	case ELimenGameplayActorState::InGameplay:
		{
			SetActorHiddenInGame(false);
			SetActorEnableCollision(true);	
		}
		break;
	case ELimenGameplayActorState::InGameplayWithCollisionDisabled:
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
