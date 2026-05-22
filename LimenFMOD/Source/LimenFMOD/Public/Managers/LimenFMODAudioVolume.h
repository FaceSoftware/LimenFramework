// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODBlueprintStatics.h"
#include "Actors/LimenGameplayActor.h"
#include "LimenFMODAudioVolume.generated.h"

class UBoxComponent;

UCLASS(DisplayName="Limen FMOD Audio Volume")
class LIMENFMOD_API ALimenFMODAudioVolume : public ALimenGameplayActor
{
	GENERATED_BODY()

public:
	ALimenFMODAudioVolume();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FMOD Audio Volume")
	TObjectPtr<UBoxComponent> VolumeBounds;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FMOD Audio Volume")
	TArray<TSubclassOf<AActor>> ActorsAllowedToTrigger;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FMOD Audio Volume")
	TObjectPtr<UFMODEvent> AudioEvent;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="FMOD Audio Volume", meta=(ClampMin="0"))
	int32 Priority;

	UFUNCTION()
	virtual void BoundsBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
									const FHitResult& SweepResult);
	UFUNCTION()
	virtual void BoundsEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
								  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	bool CanActorTriggerThis(const AActor* Test) const;

private:
	FFMODEventInstance EventInstance;
	TArray<TWeakObjectPtr<ALimenFMODAudioVolume>> IntersectingVolumes;

	void StartPlayingEvent();
	void StopPlayingEvent();
};
