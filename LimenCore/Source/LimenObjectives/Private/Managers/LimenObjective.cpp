// Copyright Face Software. All Rights Reserved.


#include "Managers/LimenObjective.h"

#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"


void FObjectiveData::operator=(const FObjectiveData& Other)
{
	UniqueName = Other.UniqueName;
	Title = Other.Title;
	Description = Other.Description;
	State = Other.State;
	CurrentProgress = Other.CurrentProgress;
	MaxProgress = Other.MaxProgress;
}

ALimenObjective::ALimenObjective()
{
	UniqueName = TEXT("DefaultObjectiveName");
	Title = FText::FromString(TEXT("DefaultObjectiveTitle"));
	Description = FText::FromName(TEXT("DefaultObjectiveDescription"));
}

void ALimenObjective::BindToPlayer(APlayerController* PlayerController, APawn* Pawn)
{
	if (PlayerController == nullptr)
	{
		return;
	}

	if (ObjectiveData.State != EObjectiveState::NotStarted)
	{
		// Prevent starting the same objective twice
		return;
	}
	
	BoundPlayerController = PlayerController;
	BoundPawn = Pawn;
	
	FObjectiveData NewData = GetObjectiveData();
	NewData.State = EObjectiveState::InProgress;
	ObjectiveData = NewData;
	ObjectiveStarted(PlayerController, Pawn);
	OnObjectiveAdded.Broadcast(this, NewData);
}

void ALimenObjective::BindToCurrentPlayer()
{
	APlayerController* Controller = GetWorld()->GetFirstPlayerController();
	check(Controller != nullptr);
	APawn* Pawn = Controller->GetPawn();
	// You can bind to a player not currently possessing a pawn, but you must bind it to a valid player controller
	
	BindToPlayer(Controller, Pawn);
}

const FObjectiveData& ALimenObjective::GetObjectiveData() const
{
	return ObjectiveData;
}

APlayerController* ALimenObjective::GetBoundPlayerController() const
{
	return BoundPlayerController.Get();
}

APawn* ALimenObjective::GetBoundPawn() const
{
	return BoundPawn.Get();
}

void ALimenObjective::LoadObjectiveForPlayer(APlayerController* PlayerController, APawn* Pawn,
	const FObjectiveData& NewData)
{
	BoundPlayerController = PlayerController;
	BoundPawn = Pawn;
	ObjectiveData = NewData;
	
	OnObjectiveLoaded.Broadcast(this, ObjectiveData);
}

bool ALimenObjective::IsCompleted() const
{
	return ObjectiveData.State == EObjectiveState::Completed;
}

const FObjectiveData& ALimenObjective::GetData() const
{
	return ObjectiveData;
}

void ALimenObjective::ResetObjective()
{
	ObjectiveData = FObjectiveData();
	BoundPawn = nullptr;
	BoundPlayerController = nullptr;
	
	OnObjectiveReset.Broadcast(this, ObjectiveData);
}

bool ALimenObjective::ShouldSaveData() const
{
	return true;
}

bool ALimenObjective::ShouldLoadData() const
{
	return true;
}

void ALimenObjective::PreDataSaved()
{
}

void ALimenObjective::PostDataSaved()
{
}

void ALimenObjective::PreDataLoaded()
{
}

void ALimenObjective::PostDataLoaded()
{
}

void ALimenObjective::CompleteObjective()
{
	ObjectiveData.State = EObjectiveState::Completed;
	ObjectiveCompleted(BoundPlayerController.Get(), BoundPawn.Get());
	OnObjectiveCompleted.Broadcast(this, ObjectiveData);
}

void ALimenObjective::Start()
{
	Super::Start();

	ObjectiveData.UniqueName = UniqueName;
	ObjectiveData.Title = Title;
	ObjectiveData.Description = Description;
}
