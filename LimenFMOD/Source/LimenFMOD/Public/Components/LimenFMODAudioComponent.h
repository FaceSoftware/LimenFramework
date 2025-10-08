// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FMODAmbientSound.h"
#include "LimenFMODAudioComponent.generated.h"


enum class EPauseReason : uint8;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENFMOD_API ULimenFMODAudioComponent : public UFMODAudioComponent
{
	GENERATED_BODY()

public:
	ULimenFMODAudioComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void SetPausePlaybackWithWorld(const bool bPause);

protected:
	UPROPERTY(EditAnywhere)
	bool bPausePlaybackWithWorld;

private:
	UFUNCTION()
	void GamePausedStateChanged(const bool bIsPaused);
};

UCLASS()
class LIMENFMOD_API ALimenFMODAmbientSound : public AFMODAmbientSound
{
	GENERATED_BODY()

public:
	explicit ALimenFMODAmbientSound(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
};
