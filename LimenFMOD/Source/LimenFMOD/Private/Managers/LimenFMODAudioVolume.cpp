// Copyright © FaceSoftware. All rights reserved.


#include "Managers/LimenFMODAudioVolume.h"


// Sets default values
ALimenFMODAudioVolume::ALimenFMODAudioVolume()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALimenFMODAudioVolume::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALimenFMODAudioVolume::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

