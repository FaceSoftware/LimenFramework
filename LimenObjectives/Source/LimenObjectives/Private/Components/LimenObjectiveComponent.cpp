// Copyright Face Software. All Rights Reserved.


#include "Components/LimenObjectiveComponent.h"

#include "LogMacros/LimenLogMacros.h"
#include "LogMacros/LimenObjectivesLogMacros.h"


ULimenObjectiveComponent::ULimenObjectiveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool ULimenObjectiveComponent::TrackObjective(ALimenObjective* NewObjective)
{
	check(NewObjective)
	
	if (ObjectiveAlreadyExist(NewObjective))
	{
		LIMEN_LOG(LogLimenObjectives, Warning, this, "Attempted to add a duplicate objective");
		return false;
	}

	ObjectivesList.Push(NewObjective);

	NewObjective->OnObjectiveUpdated.AddUniqueDynamic(this, &ThisClass::ObjectiveUpdated);
	NewObjective->OnObjectiveCompleted.AddUniqueDynamic(this, &ThisClass::ObjectiveCompleted);


	OnNewObjectiveAdded.Broadcast(NewObjective);

	LIMEN_LOG(LogLimenObjectives, Log, this, "Added new objective")
	return true;
}

bool ULimenObjectiveComponent::IsTrackingObjective(ALimenObjective* Test) const
{
	return ObjectivesList.Contains(Test);
}

void ULimenObjectiveComponent::LoadObjectives(const TArray<ALimenObjective*>& InObjectives)
{
	ResetObjectives();
	
	ObjectivesList.Reserve(InObjectives.Num());
	
	for (ALimenObjective* Objective : InObjectives)
	{		
		ObjectivesList.Push(Objective);
	}
	
	OnObjectivesLoaded.Broadcast(this);
}

const TArray<ALimenObjective*>& ULimenObjectiveComponent::GetObjectives() const
{
	return ObjectivesList;
}

void ULimenObjectiveComponent::UntrackObjective(ALimenObjective* NewObjective)
{
	ObjectivesList.Remove(NewObjective);
	NewObjective->Reset();
}

void ULimenObjectiveComponent::ResetObjectives()
{
	for (ALimenObjective* Objective : ObjectivesList)
	{
		Objective->Reset();
	}

	ObjectivesList.Empty(0);
}

bool ULimenObjectiveComponent::ObjectiveAlreadyExist(const ALimenObjective* InTest) const
{
	check(InTest)
	
	for (const auto Objective : ObjectivesList)
	{
		if (Objective == InTest)
			return true;
	}

	return false;
}

void ULimenObjectiveComponent::ObjectiveUpdated(ALimenObjective* InObjective, const FObjectiveData& NewData)
{
	OnObjectiveUpdated.Broadcast(InObjective);
}

void ULimenObjectiveComponent::ObjectiveCompleted(ALimenObjective* InObjective, const FObjectiveData& NewData)
{
	OnObjectiveCompleted.Broadcast(InObjective);
}
