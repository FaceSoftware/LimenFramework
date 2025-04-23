// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayActor.h"

#include "TimerManager.h"
#include "Engine/World.h"
#include "Net/UnrealNetwork.h"


ALimenGameplayActor::ALimenGameplayActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsRemovedFromGameplay = false;
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

	DOREPLIFETIME_WITH_PARAMS_FAST(ALimenGameplayActor, bIsRemovedFromGameplay, Params);
}

void ALimenGameplayActor::RemoveFromGameplay()
{
	if (bIsRemovedFromGameplay)
	{
		return;
	}
	
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsRemovedFromGameplay = true;
	SetReplicates(false);
}

void ALimenGameplayActor::AddToGameplay()
{
	if (!bIsRemovedFromGameplay)
	{
		return;
	}
	
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	bIsRemovedFromGameplay = false;
	SetReplicates(true);
}

bool ALimenGameplayActor::IsRemovedFromGameplay() const
{
	return bIsRemovedFromGameplay;
}

void ALimenGameplayActor::OnRep_IsRemovedFromGameplay()
{
	if (bIsRemovedFromGameplay)
	{
		RemoveFromGameplay();
	}
	else
	{
		AddToGameplay();
	}
}
