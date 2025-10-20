// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayActor.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ALimenGameplayActor::ALimenGameplayActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	GameplayState = ELimenGameplayActorState::Undefined;
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ALimenGameplayActor:: BeginPlay()
{
	Super::BeginPlay();
}

void ALimenGameplayActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ALimenGameplayActor, GameplayState, FDoRepLifetimeParams(
		COND_None, REPNOTIFY_OnChanged, true));
}

void ALimenGameplayActor::RemoveFromGameplay()
{
	check(HasAuthority())
	if (IsRemovedFromGameplay())
	{
		return;
	}

	GameplayState = ELimenGameplayActorState::OutOfGameplay;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GameplayState, this);
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
}

void ALimenGameplayActor::AddToGameplay(const bool bEnableCollision)
{
	check(HasAuthority())
	if (bEnableCollision && GameplayState == ELimenGameplayActorState::InGameplay ||
	   !bEnableCollision && GameplayState == ELimenGameplayActorState::InGameplayWithCollisionDisabled)
	{
		return;
	}

	GameplayState = bEnableCollision ? ELimenGameplayActorState::InGameplay : ELimenGameplayActorState::InGameplayWithCollisionDisabled;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, GameplayState, this);
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(bEnableCollision);
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
