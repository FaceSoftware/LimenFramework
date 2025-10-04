// Copyright © FaceSoftware. All rights reserved.


#include "Managers/LimenFMODAudioVolume.h"

#include "CompGeom/FitOrientedBox3.h"
#include "Components/BoxComponent.h"


ALimenFMODAudioVolume::ALimenFMODAudioVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	VolumeBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBounds"));
	RootComponent = VolumeBounds;

	Priority = 0;
}

void ALimenFMODAudioVolume::BeginPlay()
{
	VolumeBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VolumeBounds->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::BoundsBeginOverlap);
	VolumeBounds->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::BoundsEndOverlap);

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		VolumeBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		VolumeBounds->UpdateBodySetup();
	});

	Super::BeginPlay();
}

void ALimenFMODAudioVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	VolumeBounds->OnComponentBeginOverlap.RemoveAll(this);
	VolumeBounds->OnComponentEndOverlap.RemoveAll(this);

	UFMODBlueprintStatics::EventInstanceStop(EventInstance, true);

	Super::EndPlay(EndPlayReason);
}

void ALimenFMODAudioVolume::BoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
											   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
											   const FHitResult& SweepResult)
{
	if (auto* OtherAudioVolume = Cast<ALimenFMODAudioVolume>(OtherActor))
	{
		if (OtherAudioVolume->Priority < Priority)
		{
			IntersectingVolumes.Push(TWeakObjectPtr(OtherAudioVolume));
		}
	}

	if (!CanActorTriggerThis(OtherActor)) return;
	StartPlayingEvent();

	for (const auto& Volume : IntersectingVolumes)
	{
		Volume->StopPlayingEvent();
	}
}

void ALimenFMODAudioVolume::BoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
											 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (auto* OtherAudioVolume = Cast<ALimenFMODAudioVolume>(OtherActor))
	{
		if (OtherAudioVolume->Priority < Priority)
		{
			IntersectingVolumes.Remove(TWeakObjectPtr(OtherAudioVolume));
		}
	}

	if (!CanActorTriggerThis(OtherActor)) return;
	StopPlayingEvent();

	int32 HighestPriority = -1;
	TWeakObjectPtr<ALimenFMODAudioVolume> VolumeToPlay = nullptr;
	for (const auto& Volume : IntersectingVolumes)
	{
		if (Volume.IsValid() && Volume->Priority > HighestPriority)
		{
			HighestPriority = Volume->Priority;
			VolumeToPlay = Volume;
		}
	}
	if (VolumeToPlay.IsValid()) VolumeToPlay->StartPlayingEvent();
}

bool ALimenFMODAudioVolume::CanActorTriggerThis(const AActor* Test) const
{
	return ActorsAllowedToTrigger.IsEmpty() || Test && ActorsAllowedToTrigger.Contains(Test->GetClass());
}

void ALimenFMODAudioVolume::StartPlayingEvent()
{
	EventInstance = UFMODBlueprintStatics::PlayEvent2D(this, AudioEvent.Get(), true);
}

void ALimenFMODAudioVolume::StopPlayingEvent()
{
	UFMODBlueprintStatics::EventInstanceStop(EventInstance, true);
}
