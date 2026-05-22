// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MovieSceneSequencePlaybackSettings.h"
#include "Actors/LimenGameplayActor.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LevelSequencePlayer.h"
#include "LimenLevelSequenceTrigger.generated.h"


class ALevelSequenceActor;
class ULevelSequence;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLimenLevelSequenceTriggered, AActor*, TriggeringActor);

UCLASS(Blueprintable)
class LIMENCINEMATICS_API ALimenLevelSequenceTrigger : public ALimenGameplayActor, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLimenLevelSequenceTriggered OnTriggered;

	ALimenLevelSequenceTrigger();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	void SetTriggerEnabled(const bool bNewEnabled);
	UFUNCTION(BlueprintCallable)
	bool IsEnabled() const;

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;
	virtual FName GetUniqueDeterministicId() const override;

protected:
	inline static ECollisionEnabled::Type CollisionEnabledType = ECollisionEnabled::QueryAndPhysics;
	inline static ECollisionEnabled::Type CollisionDisabledType = ECollisionEnabled::NoCollision;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UBoxComponent> TriggerBox;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger")
	FName UniqueId;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger")
	TArray<TSubclassOf<AActor>> ActorsAllowedToTrigger;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger")
	TObjectPtr<ULevelSequence> LevelSequenceToPlay;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger")
	FMovieSceneSequencePlaybackSettings SequencePlaybackSettings;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger")
	bool bDestroyAfterFinish;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger", meta=(EditCondition="!bDestroyAfterFinish"))
	bool bAllowMultipleExecution;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger")
	bool bLoadSequenceOnLevelStart;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Level Sequence Trigger")
	bool bStartEnabled;

	UFUNCTION()
	virtual void TriggerBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
										UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
										const FHitResult& SweepResult);

	virtual void TriggerEnabledStateChanged(const bool bEnabled);
	virtual void SequenceFinished();

private:
	UPROPERTY(SaveGame)
	bool bWasTriggered;
	bool bIsTriggerEnabled;
	TStrongObjectPtr<ULevelSequencePlayer> SequencePlayerStrongPtr;
	TWeakObjectPtr<ALevelSequenceActor> SequenceActorWeakPtr;

	void LoadSequence();
	void UnloadSequence();
	void PlaySequence();
	bool IsSequenceLoaded() const;
};
