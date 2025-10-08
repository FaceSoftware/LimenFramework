// Copyright Face Software. All Rights Reserved.


#include "SavesHandlers/LimenSavesHandler.h"

#include "LogMacros/LimenLogMacros.h"
#include "SaveGames/LimenSaveData.h"


bool ULimenSavesHandler::DoesListContainDuplicatedDeterministicIds(TArray<FActorSaveData>& InData) const
{
	TSet<FName> SeenIds;

	for (const FActorSaveData& Data : InData)
	{
		const FName Id = Data.GetDeterministicId();
		if (Id.IsNone())
		{
			LIMEN_LOG(LogLimen, Warning, this, TEXT("Actor save data missing DeterministicId: %s"), *Data.GetActorName().ToString());
			continue;
		}

		if (SeenIds.Contains(Id))
		{
			LIMEN_LOG(LogLimen, Error, this, TEXT("Duplicate DeterministicId found: %s"), *Id.ToString());
			return true;
		}

		SeenIds.Add(Id);
	}

	return false;
}

bool ULimenSavesHandler::ShouldSaveData() const
{
	return true;
}

bool ULimenSavesHandler::ShouldLoadData() const
{
	return true;
}

void ULimenSavesHandler::PreDataSaved()
{
}

void ULimenSavesHandler::PostDataSaved()
{
}

void ULimenSavesHandler::PreDataLoaded()
{
}

void ULimenSavesHandler::PostDataLoaded()
{
}

FName ULimenSavesHandler::GetUniqueDeterministicId() const
{
	return NAME_None;
}

bool ULimenSavesHandler::SaveDataFrom(UWorld* World)
{
	WorldContext = World;
	return WorldContext.IsValid();
}

bool ULimenSavesHandler::LoadDataTo(UWorld* World)
{
	WorldContext = World;
	return WorldContext.IsValid();
}

UWorld* ULimenSavesHandler::GetWorld() const
{
	ensureMsgf(WorldContext.IsValid(), TEXT("ULimenSavesHandler::GetWorld() called before SaveDataFrom/LoadDataTo set WorldContext."));
	return WorldContext.Get();
}
