// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayActor.h"


ALimenGameplayActor::ALimenGameplayActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsRemovedFromGameplay = false;
}

void ALimenGameplayActor:: BeginPlay()
{
	Super::BeginPlay();
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::Initialize);
}

void ALimenGameplayActor::Initialize()
{
	OnActorInitialized.Broadcast(this);
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
}

bool ALimenGameplayActor::IsRemovedFromGameplay() const
{
	return bIsRemovedFromGameplay;
}
