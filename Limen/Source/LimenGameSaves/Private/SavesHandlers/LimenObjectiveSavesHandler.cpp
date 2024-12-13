// Copyright Face Software. All Rights Reserved.


#include "SavesHandlers/LimenObjectiveSavesHandler.h"

#include "Components/LimenObjectiveComponent.h"
#include "Managers/LimenObjective.h"
#include "SaveGames/LimenSaveData.h"


void ULimenObjectiveSavesHandler::SaveDataFrom(UWorld* World)
{
	const ULimenObjectiveComponent* PlayerObjectivesComponent = World->GetFirstPlayerController()->GetPawn()->GetComponentByClass<ULimenObjectiveComponent>();
	const TArray<ALimenObjective*>& Objectives = PlayerObjectivesComponent->GetObjectives();
	for (ALimenObjective* Objective : Objectives)
	{
		ObjectivesSaveData.Push(FActorSaveData(Objective));
	}
}

void ULimenObjectiveSavesHandler::LoadDataTo(UWorld* World)
{
	ULimenObjectiveComponent* PlayerObjectivesComponent = World->GetFirstPlayerController()->GetPawn()->GetComponentByClass<ULimenObjectiveComponent>();

	TArray<ALimenObjective*> SavedPlayerObjectives;
	for (TActorIterator<ALimenObjective> It(World); It; ++It)
	{
		for (const FActorSaveData& ObjectiveSave : ObjectivesSaveData)
		{
			if (It->GetClass() == ObjectiveSave.GetActorClass())
			{
				ObjectiveSave.LoadData(*It);
				SavedPlayerObjectives.Push(*It);
			}	
		}
	}

	PlayerObjectivesComponent->LoadObjectives(SavedPlayerObjectives);
}
