// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/DynamicLevelSequenceComponent.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Engine/World.h"


UDynamicLevelSequenceComponent::UDynamicLevelSequenceComponent()
{
	bIsPlaying = false;
}

void UDynamicLevelSequenceComponent::BeginPlay()
{
	Super::BeginPlay();

	LevelSequence = TStrongObjectPtr(LevelSequenceAsset.LoadSynchronous());
}

void UDynamicLevelSequenceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	LevelSequence.Reset();
	verify(SequenceActor->Destroy())
	SequenceActor.Reset();
	
	Super::EndPlay(EndPlayReason);
}

void UDynamicLevelSequenceComponent::PlaySequence()
{
	if (!LevelSequence.IsValid())
	{
		return;
	}
	
	InitializeSequence();
	GetSequencePlayer()->Play();
}

void UDynamicLevelSequenceComponent::StopSequence()
{
	if (!LevelSequence.IsValid() || !SequenceActor.IsValid())
	{
		return;
	}
	
	SequenceActor->GetSequencePlayer()->Stop();
	
	bIsPlaying = SequenceActor->GetSequencePlayer()->IsPlaying();
}

bool UDynamicLevelSequenceComponent::IsPlaying() const
{
	return bIsPlaying;
}

ULevelSequencePlayer* UDynamicLevelSequenceComponent::GetSequencePlayer() const
{
	return SequenceActor.IsValid() ? SequenceActor->GetSequencePlayer() : nullptr;
}

void UDynamicLevelSequenceComponent::SequencePlay()
{
}

void UDynamicLevelSequenceComponent::SequencePlayReverse()
{
}

void UDynamicLevelSequenceComponent::SequenceStop()
{
	DeinitializeSequence();
}

void UDynamicLevelSequenceComponent::SequencePause()
{
}

void UDynamicLevelSequenceComponent::SequenceFinished()
{
	DeinitializeSequence();
}

void UDynamicLevelSequenceComponent::InitializeSequence()
{
	if (!LevelSequence.IsValid())
	{
		return;
	}
	
	if (!SequenceActor.IsValid())
	{
		SequenceActor = GetWorld()->SpawnActor<ALevelSequenceActor>();
		check(SequenceActor.IsValid())
	}
	
	SequenceActor->SetSequence(LevelSequence.Get());
	SequenceActor->GetSequencePlayer()->OnPlay.AddUniqueDynamic(this, &UDynamicLevelSequenceComponent::SequencePlay);
	SequenceActor->GetSequencePlayer()->OnPlayReverse.AddUniqueDynamic(this, &UDynamicLevelSequenceComponent::SequencePlayReverse);
	SequenceActor->GetSequencePlayer()->OnStop.AddUniqueDynamic(this, &UDynamicLevelSequenceComponent::SequenceStop);
	SequenceActor->GetSequencePlayer()->OnPause.AddUniqueDynamic(this, &UDynamicLevelSequenceComponent::SequencePause);
	SequenceActor->GetSequencePlayer()->OnFinished.AddUniqueDynamic(this, &UDynamicLevelSequenceComponent::SequenceFinished);
}

void UDynamicLevelSequenceComponent::DeinitializeSequence()
{
	SequenceActor->GetSequencePlayer()->OnPlay.RemoveDynamic(this, &UDynamicLevelSequenceComponent::SequencePlay);
	SequenceActor->GetSequencePlayer()->OnPlayReverse.RemoveDynamic(this, &UDynamicLevelSequenceComponent::SequencePlayReverse);
	SequenceActor->GetSequencePlayer()->OnStop.RemoveDynamic(this, &UDynamicLevelSequenceComponent::SequenceStop);
	SequenceActor->GetSequencePlayer()->OnPause.RemoveDynamic(this, &UDynamicLevelSequenceComponent::SequencePause);
	SequenceActor->GetSequencePlayer()->OnFinished.RemoveDynamic(this, &UDynamicLevelSequenceComponent::SequenceFinished);
	
	verify(SequenceActor->Destroy())
	SequenceActor.Reset();
}
