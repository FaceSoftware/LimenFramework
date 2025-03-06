// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenArchiveSubsystem.h"

#include "LimenStorageSaveData.h"
#include "Archives/LimenArchive.h"


ULimenArchiveSubsystem::ULimenArchiveSubsystem()
{
	SaveDataName = TEXT("ArchivesData");
}

bool ULimenArchiveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}

	return true;
}

void ULimenArchiveSubsystem::AddArchive(ULimenArchive* NewArchive)
{
	check(IsValid(NewArchive));
	AddItem(NewArchive);
	Save();
	
	NewArchive->OnArchiveRead.AddUniqueDynamic(this, &ThisClass::Save);
}

void ULimenArchiveSubsystem::Load_Internal()
{
	const uint32 NumberOfArchives = GetCurrentSaveData()->GetObjectSaveDataCount();
	for (uint32 i = 0; i < NumberOfArchives; ++i)
	{
		FObjectSaveData SaveData;
		if (!GetCurrentSaveData()->GetObjectSaveData(i, SaveData))
		{
			continue;
		}
		
		// Create the archive
		const TSoftClassPtr<UObject>& ArchiveClassSoftPtr = SaveData.GetObjectClass();
		if (ArchiveClassSoftPtr.IsNull())
		{
			continue;
		}
		
		ULimenArchive* Archive = NewObject<ULimenArchive>(this, ArchiveClassSoftPtr.Get());
		check(Archive != nullptr)

		// Load data into the specific archive
		SaveData.LoadData(Archive);

		// Add the item again into the list and bind events (without saving the list!!)
		AddItem(Archive);
		Archive->OnArchiveRead.AddUniqueDynamic(this, &ThisClass::Save);
	}
}
