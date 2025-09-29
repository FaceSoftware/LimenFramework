// Copyright © FaceSoftware. All rights reserved.


#include "Managers/LimenFMODAudioVolume.h"

#include "Components/BoxComponent.h"


ALimenFMODAudioVolume::ALimenFMODAudioVolume()
{
	PrimaryActorTick.bCanEverTick = false;

	VolumeBounds = CreateDefaultSubobject<UBoxComponent>(TEXT("VolumeBounds"));
	RootComponent = VolumeBounds;
}

void ALimenFMODAudioVolume::BeginPlay()
{
	EventInstance = UFMODBlueprintStatics::PlayEvent2D(this, AudioEvent.Get(), false);


	VolumeBounds->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	VolumeBounds->OnComponentBeginOverlap.AddUniqueDynamic(this, &ThisClass::BoundsBeginOverlap);
	VolumeBounds->OnComponentEndOverlap.AddUniqueDynamic(this, &ThisClass::BoundsEndOverlap);

	GetWorld()->GetTimerManager().SetTimerForNextTick([this]
	{
		VolumeBounds->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		VolumeBounds->UpdateCollisionProfile();
	});

	Super::BeginPlay();
}

void ALimenFMODAudioVolume::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	VolumeBounds->OnComponentBeginOverlap.RemoveAll(this);
	VolumeBounds->OnComponentEndOverlap.RemoveAll(this);

	EventInstance.Instance->release();

	Super::EndPlay(EndPlayReason);
}

void ALimenFMODAudioVolume::BoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
											   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
											   const FHitResult& SweepResult)
{
	if (!CanActorTriggerThis(OtherActor)) return;

	EventInstance.Instance->start();
}

void ALimenFMODAudioVolume::BoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
											 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!CanActorTriggerThis(OtherActor)) return;

	EventInstance.Instance->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
}

bool ALimenFMODAudioVolume::CanActorTriggerThis(const AActor* Test) const
{
	return ActorsAllowedToTrigger.IsEmpty() || Test && ActorsAllowedToTrigger.Contains(Test->GetClass());
}
