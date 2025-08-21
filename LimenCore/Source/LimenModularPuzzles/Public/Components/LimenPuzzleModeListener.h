// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenPuzzleModeListener.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPuzzleModeStateDelegate, const bool, bPuzzleModeActive, AActor*, PuzzleActor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENMODULARPUZZLES_API ULimenPuzzleModeListener : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPuzzleModeStateDelegate OnPuzzleModeChanged;

	ULimenPuzzleModeListener();

	UFUNCTION(BlueprintCallable)
	void SetPuzzleMode(const bool bPuzzleMode, AActor* PuzzleActor = nullptr);
	UFUNCTION(BlueprintCallable)
	bool IsPuzzleModeActive() const;

protected:

private:
	bool bCurrentPuzzleMode;
	TWeakObjectPtr<AActor> CurrentPuzzleActor;
};
