// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenDynamicLevelSequenceComponent.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "TimerManager.h"
#include "Engine/World.h"


ULimenDynamicLevelSequenceComponent::ULimenDynamicLevelSequenceComponent()
{
	bIsPlaying = false;
	bAutoActivate = true;
}

void ULimenDynamicLevelSequenceComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnSetupComponent();
	
	Super::EndPlay(EndPlayReason);
}

void ULimenDynamicLevelSequenceComponent::Activate(bool bReset)
{
	if (!GetWorld()->IsGameWorld())
	{
		return;
	}
	
	const bool bShouldActivate = ShouldActivate();
	Super::Activate(bReset);
	
	if (bReset)
	{
		UnSetupComponent();
		SetupComponent();
	}
	else if (bShouldActivate) 
	{
		SetupComponent();
	}
}

void ULimenDynamicLevelSequenceComponent::Deactivate()
{
	const bool bShouldDeactivate = !ShouldActivate();
	Super::Deactivate();

	if (bShouldDeactivate)
	{
		UnSetupComponent();
	}
}

bool ULimenDynamicLevelSequenceComponent::ShouldActivate() const
{
	if (!Super::ShouldActivate())
	{
		return false;
	}

	return !LevelSequenceAsset.IsNull();
}

void ULimenDynamicLevelSequenceComponent::PlaySequence()
{
	if (!IsActive())
	{
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::PlaySequenceInternal);
}

void ULimenDynamicLevelSequenceComponent::StopSequence()
{
	if (!IsActive())
	{
		return;
	}
	
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

void ULimenDynamicLevelSequenceComponent::SetupComponent()
{
	check(!SequenceActor.IsValid())
	check(!LevelSequence.IsValid())
	
	LevelSequence = TStrongObjectPtr(LevelSequenceAsset.LoadSynchronous());
	SequenceActor = GetWorld()->SpawnActor<ALevelSequenceActor>();
	
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

void ULimenDynamicLevelSequenceComponent::UnSetupComponent()
{	
	if (LevelSequence.IsValid()) LevelSequence.Reset();
	
	if (SequenceActor.IsValid())
	{
		SequenceActor->GetSequencePlayer()->OnPlay.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePlay);
		SequenceActor->GetSequencePlayer()->OnPlayReverse.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePlayReverse);
		SequenceActor->GetSequencePlayer()->OnStop.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequenceStop);
		SequenceActor->GetSequencePlayer()->OnPause.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequencePause);
		SequenceActor->GetSequencePlayer()->OnFinished.RemoveDynamic(this, &ULimenDynamicLevelSequenceComponent::SequenceFinished);
		
		verify(SequenceActor->Destroy());
		SequenceActor.Reset();
	}
}

void ULimenDynamicLevelSequenceComponent::PlaySequenceInternal()
{
	GetSequencePlayer()->Play();
	bIsPlaying = SequenceActor->GetSequencePlayer()->IsPlaying();
}
