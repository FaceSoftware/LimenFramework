// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenPuzzleModeListener.h"


ULimenPuzzleModeListener::ULimenPuzzleModeListener()
{
	PrimaryComponentTick.bCanEverTick = false;
	bCurrentPuzzleMode = false;
}

void ULimenPuzzleModeListener::SetPuzzleMode(const bool bPuzzleMode, AActor* PuzzleActor)
{
	if (CurrentPuzzleActor == PuzzleActor) return;

#if WITH_EDITOR
	if (bPuzzleMode)
	{
		check(PuzzleActor != nullptr);
	}
#endif WITH_EDITOR

	CurrentPuzzleActor.Reset();
	bCurrentPuzzleMode = bPuzzleMode;
	CurrentPuzzleActor = bPuzzleMode ? PuzzleActor : nullptr;
	OnPuzzleModeChanged.Broadcast(bPuzzleMode, PuzzleActor);
}

bool ULimenPuzzleModeListener::IsPuzzleModeActive() const
{
	return bCurrentPuzzleMode;
}
