// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenGameplayActor.h"

#include "Net/UnrealNetwork.h"


ALimenGameplayActor::ALimenGameplayActor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bIsRemovedFromGameplay = false;
}

void ALimenGameplayActor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(ALimenGameplayActor, bIsRemovedFromGameplay, COND_InitialOnly);
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
	SetActorHiddenInGame(true);
	SetActorEnableCollision(false);
	bIsRemovedFromGameplay = true;
}

void ALimenGameplayActor::AddToGameplay()
{
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
	bIsRemovedFromGameplay = false;
}

bool ALimenGameplayActor::IsRemovedFromGameplay() const
{
	return bIsRemovedFromGameplay;
}

void ALimenGameplayActor::OnRep_bIsRemovedFromGameplay()
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
