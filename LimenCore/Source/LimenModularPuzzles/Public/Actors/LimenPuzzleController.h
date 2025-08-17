// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "Utils/PuzzleHelpers.h"
#include "LimenPuzzleController.generated.h"


class ULimenPuzzleElementComponent;
class UPuzzleDefinition;

UCLASS()
class LIMENMODULARPUZZLES_API ALimenPuzzleController : public ALimenGameplayManager
{
	GENERATED_BODY()

public:
	ALimenPuzzleController();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;


protected:
	UPROPERTY(EditAnywhere, Category="Puzzle")
	TObjectPtr<UPuzzleDefinition> PuzzleDefinition;

	UFUNCTION(BlueprintImplementableEvent)
	void PuzzleSolved();
	UFUNCTION(BlueprintImplementableEvent)
	void PuzzleUnsolved();

	UFUNCTION(BlueprintCallable)
	float GetCompletionNormalized() const;

	UFUNCTION()
	virtual void OnRep_PuzzleState();
	UFUNCTION()
	virtual void AnyPuzzleElementChanged();

private:
	UPROPERTY(ReplicatedUsing=OnRep_PuzzleState)
	bool bSolved = false;

	TMap<FName, bool> CachedStateValues;
	TStrongObjectPtr<UPuzzleDefinition> PuzzleDefinitionPtr;
	TMap<FName, FTimerGateState> TimerStates;
	TArray<TWeakObjectPtr<ULimenPuzzleElementComponent>> PuzzleElements;

	void BindElements();
	bool EvalElementOrNode(const FName& Id);
	bool EvalCached(const FName& Id);
	void Recompute();
	void ResetTimerGate(FName NodeId);
	bool TimerGate(const FName& NodeId, const TArray<bool>& Inputs);

	UFUNCTION()
	void AnyComponentActivated(UActorComponent* Component, bool bReset);
	UFUNCTION()
	void AnyComponentDeactivated(UActorComponent* Component);
};
