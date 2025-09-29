// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "LimenFMODAudioVolume.generated.h"

UCLASS()
class LIMENFMOD_API ALimenFMODAudioVolume : public ALimenGameplayManager
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ALimenFMODAudioVolume();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
