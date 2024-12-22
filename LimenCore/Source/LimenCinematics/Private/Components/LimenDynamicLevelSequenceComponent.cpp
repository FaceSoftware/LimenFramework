// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenDynamicLevelSequenceComponent.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "TimerManager.h"
#include "Engine/World.h"


ULimenDynamicLevelSequenceComponent::ULimenDynamicLevelSequenceComponent()
{
	bIsPlaying = false;
}

void ULimenDynamicLevelSequenceComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULimenDynamicLevelSequenceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SequenceActor.IsValid())
	{
		verify(SequenceActor->Destroy())
	}

	SequenceActor.Reset();
	LevelSequence.Reset();
	SequenceActor.Reset();
	
	Super::EndPlay(EndPlayReason);
}

void ULimenDynamicLevelSequenceComponent::PlaySequence()
{
	if (LevelSequenceAsset.IsNull())
	{
		return;
	}
	
	InitializeSequence();
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		GetSequencePlayer()->Play();
	});
}

void ULimenDynamicLevelSequenceComponent::StopSequence()
{
	if (!LevelSequence.IsValid() || !SequenceActor.IsValid())
	{
		return;
	}
	
	SequenceActor->GetSequencePlayer()->Stop();
	
	bIsPlaying = SequenceActor->GetSequencePlayer()->IsPlaying();
}

bool ULimenDynamicLevelSequenceComponent::IsPlaying() const
{
	return bIsPlaying;
}

ULevelSequencePlayer* ULimenDynamicLevelSequenceComponent::GetSequencePlayer() const
{
	return SequenceActor.IsValid() ? SequenceActor->GetSequencePlayer() : nullptr;
}

void ULimenDynamicLevelSequenceComponent::SequencePlay()
{
}

void ULimenDynamicLevelSequenceComponent::SequencePlayReverse()
{
}

void ULimenDynamicLevelSequenceComponent::SequenceStop()
{
}

void ULimenDynamicLevelSequenceComponent::SequencePause()
{
}

void ULimenDynamicLevelSequenceComponent::SequenceFinished()
{
}

void ULimenDynamicLevelSequenceComponent::InitializeSequence()
{
	if (LevelSequenceAsset.IsNull())
	{
		return;
	}
	
	if (SequenceActor.IsValid())
	{
		verify(SequenceActor->Destroy());
		SequenceActor.Reset();
	}

	if (!LevelSequence.IsValid())
	{
		LevelSequence = TStrongObjectPtr(LevelSequenceAsset.LoadSynchronous());
	}

	SequenceActor = GetWorld()->SpawnActor<ALevelSequenceActor>();
	check(SequenceActor.IsValid())
	
	SequenceActor->PlaybackSettings = PlaybackSettings;
	SequenceActor->GetSequencePlayer()->SetPlaybackSettings(PlaybackSettings);
	SequenceActor->SetSequence(LevelSequence.Get());
	SequenceActor->InitializePlayer();
	
	SequenceActor->GetSequencePlayer()->OnPlay.AddUniqueDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePlay);
	SequenceActor->GetSequencePlayer()->OnPlayReverse.AddUniqueDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePlayReverse);
	SequenceActor->GetSequencePlayer()->OnStop.AddUniqueDynamic(this, &ULimenDynamicLevelSequenceComponent::SequenceStop);
	SequenceActor->GetSequencePlayer()->OnPause.AddUniqueDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePause);
	SequenceActor->GetSequencePlayer()->OnFinished.AddUniqueDynamic(this, &ULimenDynamicLevelSequenceComponent::SequenceFinished);
}

void ULimenDynamicLevelSequenceComponent::DeinitializeSequence()
{
	SequenceActor->GetSequencePlayer()->OnPlay.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePlay);
	SequenceActor->GetSequencePlayer()->OnPlayReverse.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePlayReverse);
	SequenceActor->GetSequencePlayer()->OnStop.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequenceStop);
	SequenceActor->GetSequencePlayer()->OnPause.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePause);
	SequenceActor->GetSequencePlayer()->OnFinished.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequenceFinished);
}
