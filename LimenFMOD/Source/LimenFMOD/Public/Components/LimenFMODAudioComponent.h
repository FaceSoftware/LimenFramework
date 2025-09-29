// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODAudioComponent.h"
#include "LimenFMODAudioComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENFMOD_API ULimenFMODAudioComponent : public UFMODAudioComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	ULimenFMODAudioComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
};
