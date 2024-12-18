// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UObject/StrongObjectPtr.h"
#include "DynamicLevelSequenceComponent.generated.h"


class ULevelSequencePlayer;
class ALevelSequenceActor;
class ULevelSequence;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCINEMATICS_API UDynamicLevelSequenceComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UDynamicLevelSequenceComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

	void InitializeSequence();
	void DeinitializeSequence();
};
