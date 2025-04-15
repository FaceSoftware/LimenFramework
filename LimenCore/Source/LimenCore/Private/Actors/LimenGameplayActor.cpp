// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayActor.h"

#include "TimerManager.h"
#include "Engine/World.h"


ALimenGameplayActor::ALimenGameplayActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsRemovedFromGameplay = false;
}

void ALimenGameplayActor:: BeginPlay()
{
	Super::BeginPlay();
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::Initialize_Internal);
}

void ALimenGameplayActor::Initialize()
{
	OnActorInitialized.Broadcast(this);
}

void ALimenGameplayActor::RemoveFromGameplay()
{
	check(HasAuthority())
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
	check(HasAuthority())
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

void ALimenGameplayActor::Initialize_Internal()
{
	Initialize();
	OnActorInitialized.Broadcast(this);
}
