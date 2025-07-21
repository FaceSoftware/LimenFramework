// Copyright Face Software. All Rights Reserved.


#include "Triggers/LimenTrigger.h"

#include "TimerManager.h"
#include "Components/BoxComponent.h"
#include "Engine/World.h"
#include "LogMacros/LimenLogMacros.h"


ALimenTrigger::ALimenTrigger()
{
	PrimaryActorTick.bCanEverTick = true;
	bGenerateOverlapEventsDuringLevelStreaming = true;

	bDestroyAfterFirstTrigger = true;
	CollisionComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerArea"));
	CollisionComponent->SetGenerateOverlapEvents(true);
	CollisionComponent->SetNotifyRigidBodyCollision(true);

	RootComponent = CollisionComponent;
}

void ALimenTrigger::BeginPlay()
{
	Super::BeginPlay();

	if (!CollisionComponent)
	{
		return;
	}
	
	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::ComponentBeginOverlap);
	CollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::ComponentEndOverlap);
	CollisionComponent->OnComponentHit.AddDynamic(this, &ThisClass::ComponentHit);
}

void ALimenTrigger::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                          UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
                                          const FHitResult& SweepResult)
{
	LIMEN_LOG(LogLimenCore, Log, this, TEXT("Triggered: OnBeginOverlap"))
	SetDestroyTimer();
}

void ALimenTrigger::ComponentEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	LIMEN_LOG(LogLimenCore, Log, this, TEXT("Triggered: OnEndOverlap"))
	SetDestroyTimer();
}

void ALimenTrigger::ComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                                 FVector NormalImpulse, const FHitResult& Hit)
{
	LIMEN_LOG(LogLimenCore, Log, this, TEXT("Triggered: OnHit"))
	SetDestroyTimer();
}

void ALimenTrigger::SetDestroyTimer()
{
	DestroyTimer = GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyHandler);
}

void ALimenTrigger::DestroyHandler()
{
	Destroy();
}
