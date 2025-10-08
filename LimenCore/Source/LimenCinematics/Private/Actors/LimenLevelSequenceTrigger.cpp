// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenLevelSequenceTrigger.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "Components/BoxComponent.h"


ALimenLevelSequenceTrigger::ALimenLevelSequenceTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	bDestroyAfterFinish = true;
	bAllowMultipleExecution = false;
	bLoadSequenceOnLevelStart = true;
	bStartEnabled = true;
	if (UniqueId == NAME_None)
	{
		UniqueId = *FGuid::NewGuid().ToString();
	}

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionEnabled(CollisionEnabledType);
	RootComponent = TriggerBox;
}

void ALimenLevelSequenceTrigger::BeginPlay()
{
	Super::BeginPlay();

	TriggerBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::TriggerBoxBeginOverlap);
	if (bLoadSequenceOnLevelStart)
	{
		LoadSequence();
	}
	SetTriggerEnabled(bStartEnabled);
}

void ALimenLevelSequenceTrigger::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (SequencePlayerStrongPtr.IsValid())
	{
		SequencePlayerStrongPtr->MarkAsGarbage();
		SequencePlayerStrongPtr.Reset();
	}
}

void ALimenLevelSequenceTrigger::SetTriggerEnabled(const bool bNewEnabled)
{
	bIsTriggerEnabled = bNewEnabled;
	TriggerEnabledStateChanged(bNewEnabled);
}

bool ALimenLevelSequenceTrigger::IsEnabled() const
{
	return bIsTriggerEnabled;
}

bool ALimenLevelSequenceTrigger::ShouldSaveData() const
{
	return true;
}

bool ALimenLevelSequenceTrigger::ShouldLoadData() const
{
	return true;
}

void ALimenLevelSequenceTrigger::PreDataSaved()
{
}

void ALimenLevelSequenceTrigger::PostDataSaved()
{
}

void ALimenLevelSequenceTrigger::PreDataLoaded()
{
}

void ALimenLevelSequenceTrigger::PostDataLoaded()
{
	if (!bAllowMultipleExecution && bWasTriggered || bDestroyAfterFinish && bWasTriggered)
	{
		SetTriggerEnabled(false);
		UnloadSequence();
	}
}

FName ALimenLevelSequenceTrigger::GetUniqueDeterministicId() const
{
	return UniqueId;
}

void ALimenLevelSequenceTrigger::TriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
                                                        bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) return;
	if (!OtherActor->GetClass()) return;
	if (!ActorsAllowedToTrigger.IsEmpty() && !ActorsAllowedToTrigger.Contains(OtherActor->GetClass())) return;

	bWasTriggered = true;
	if (!IsSequenceLoaded())
	{
		LoadSequence();
	}
	PlaySequence();
	OnTriggered.Broadcast(OtherActor);
}

void ALimenLevelSequenceTrigger::TriggerEnabledStateChanged(const bool bEnabled)
{
	TriggerBox->SetCollisionEnabled(bEnabled ? CollisionEnabledType : CollisionDisabledType);
}

void ALimenLevelSequenceTrigger::SequenceFinished()
{
	if (bDestroyAfterFinish)
	{
		UnloadSequence();
	}
}

void ALimenLevelSequenceTrigger::LoadSequence()
{
	if (!LevelSequenceToPlay) return;
	UnloadSequence();

	ALevelSequenceActor* SequenceActor = nullptr;
	auto* SequencePlayer = ULevelSequencePlayer::CreateLevelSequencePlayer(this,
																		  LevelSequenceToPlay,
																		  SequencePlaybackSettings,
																		  SequenceActor);
	SequenceActorWeakPtr = SequenceActor;
	SequencePlayerStrongPtr = TStrongObjectPtr(SequencePlayer);
	SequencePlayerStrongPtr->OnNativeFinished.BindUObject(this, &ThisClass::SequenceFinished);
}

void ALimenLevelSequenceTrigger::UnloadSequence()
{
	if (SequenceActorWeakPtr.IsValid())
	{
		SequenceActorWeakPtr->Destroy();
		SequenceActorWeakPtr.Reset();
	}
	if (SequencePlayerStrongPtr.IsValid())
	{
		SequencePlayerStrongPtr->OnNativeFinished.Unbind();
		SequencePlayerStrongPtr->MarkAsGarbage();
		SequencePlayerStrongPtr.Reset();
	}
}

void ALimenLevelSequenceTrigger::PlaySequence()
{
	if (!IsSequenceLoaded()) return;

	SequencePlayerStrongPtr->Play();
	SetTriggerEnabled(bAllowMultipleExecution);
}

bool ALimenLevelSequenceTrigger::IsSequenceLoaded() const
{
	return SequencePlayerStrongPtr.IsValid() && SequenceActorWeakPtr.IsValid();
}
