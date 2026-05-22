// Copyright Face Software. All Rights Reserved.


#include "Managers/LimenObjectivesManager.h"

#include "Managers/LimenObjective.h"
#include "Components/LimenObjectiveComponent.h"
#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"


ALimenObjectivesManager::ALimenObjectivesManager()
{
	
}

void ALimenObjectivesManager::Start()
{
	Super::Start();
	ObjectiveInstances.Empty(ObjectivesClassList.Num());
	
	for (const auto& ObjectiveClass : ObjectivesClassList)
	{
		ALimenObjective* NewObjective = GetWorld()->SpawnActorDeferred<ALimenObjective>(ObjectiveClass, FTransform::Identity, this);
		NewObjective->OnObjectiveAdded.AddUniqueDynamic(this, &ThisClass::ObjectiveAdded);
		NewObjective->FinishSpawning(FTransform::Identity, false);
		
		ObjectiveInstances.Push(NewObjective);
	}

	OnInitialized.Broadcast(this);
}

TArray<ALimenObjective*> ALimenObjectivesManager::LoadObjectivesForPlayer(APlayerController* Controller, APawn* Pawn,
	const TArray<TSubclassOf<ALimenObjective>>& InObjectives) const
{
	check(Controller != nullptr);
	check(Pawn != nullptr);
	
	TArray<ALimenObjective*> PlayerObjectiveInstances;
	PlayerObjectiveInstances.Reserve(InObjectives.Num());

	// Get objectives instances
	for (const TSubclassOf<ALimenObjective>& ObjectiveToLoad : InObjectives)
	{
		ALimenObjective* TempObjectivePtr = FindObjective(ObjectiveToLoad);
		check(TempObjectivePtr != nullptr);

		PlayerObjectiveInstances.Push(TempObjectivePtr);
	}

	ULimenObjectiveComponent* ObjectiveComponent = Pawn->GetComponentByClass<ULimenObjectiveComponent>();
	check(ObjectiveComponent != nullptr);
	
	ObjectiveComponent->LoadObjectives(PlayerObjectiveInstances);
	return PlayerObjectiveInstances;
}

TArray<ALimenObjective*> ALimenObjectivesManager::GetObjectivesForPlayer(const APlayerController* Controller, const APawn* Pawn) const
{
	TArray<ALimenObjective*> OutObjectives;
	for (ALimenObjective* Objective : ObjectiveInstances)
	{
		if (Objective->GetBoundPlayerController() == Controller && Objective->GetBoundPawn() == Pawn)
		{
			OutObjectives.Push(Objective);
		}
	}
	return OutObjectives;
}

const TArray<ALimenObjective*>& ALimenObjectivesManager::GetObjectivesInstances() const
{
	return ObjectiveInstances;
}

ALimenObjective* ALimenObjectivesManager::FindObjective(const TSubclassOf<ALimenObjective>& ObjectiveClass) const
{
	check(ObjectiveClass != nullptr);
	for (ALimenObjective* Objective : ObjectiveInstances)
	{
		if (Objective->GetClass() == ObjectiveClass)
		{
			return Objective;
		}
	}
	return nullptr;
}

void ALimenObjectivesManager::ObjectiveAdded(ALimenObjective* Objective, const FObjectiveData& Data)
{
	if (Objective == nullptr)
	{
		return;
	}

	check(Objective->GetBoundPawn() != nullptr);
	
	if (ULimenObjectiveComponent* ObjectiveInventory = Objective->GetBoundPawn()->GetComponentByClass<ULimenObjectiveComponent>();
		ObjectiveInventory != nullptr)
	{
		ObjectiveInventory->TrackObjective(Objective);
	}	
}
