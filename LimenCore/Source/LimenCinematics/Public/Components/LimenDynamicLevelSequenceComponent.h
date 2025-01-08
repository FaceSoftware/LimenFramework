// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Components/ActorComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "LimenDynamicLevelSequenceComponent.generated.h"


class ULevelSequencePlayer;
class ALevelSequenceActor;
class ULevelSequence;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCINEMATICS_API ULimenDynamicLevelSequenceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULimenDynamicLevelSequenceComponent();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;
	virtual bool ShouldActivate() const override;

	UFUNCTION(BlueprintCallable)
	void PlaySequence();
	UFUNCTION(BlueprintCallable)
	void StopSequence();
	
	UFUNCTION(BlueprintCallable)
	bool IsPlaying() const;

	UFUNCTION(BlueprintCallable)
	ULevelSequencePlayer* GetSequencePlayer() const;

protected:
	UPROPERTY(EditAnywhere)
	TSoftObjectPtr<ULevelSequence> LevelSequenceAsset;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Playback", meta=(ShowOnlyInnerProperties, ExposeOnSpawn))
	FMovieSceneSequencePlaybackSettings PlaybackSettings;

	UFUNCTION()
	virtual void SequencePlay();
	UFUNCTION()
	virtual void SequencePlayReverse();
	UFUNCTION()
	virtual void SequenceStop();
	UFUNCTION()
	virtual void SequencePause();
	UFUNCTION()
	virtual void SequenceFinished();

private:
	TStrongObjectPtr<ULevelSequence> LevelSequence;
	TWeakObjectPtr<ALevelSequenceActor> SequenceActor;
	bool bIsPlaying;

	void SetupComponent();
	void UnSetupComponent();
};
