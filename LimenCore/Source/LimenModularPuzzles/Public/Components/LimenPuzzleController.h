// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "Utils/PuzzleHelpers.h"
#include "LimenPuzzleController.generated.h"


class ULimenPuzzleElementComponent;
class UPuzzleDefinition;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPuzzleStateDelegate, bool, bIsSolved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPuzzleDelegate, ULimenPuzzleController*, Component);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENMODULARPUZZLES_API ULimenPuzzleController : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPuzzleStateDelegate OnPuzzleStateChanged;
	UPROPERTY(BlueprintAssignable)
	FPuzzleDelegate OnPuzzleElementUpdated;

	ULimenPuzzleController();
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintCallable)
	float GetCompletionNormalized() const;

protected:
	UPROPERTY(EditAnywhere, Category="Puzzle")
	TObjectPtr<UPuzzleDefinition> PuzzleDefinition;

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
