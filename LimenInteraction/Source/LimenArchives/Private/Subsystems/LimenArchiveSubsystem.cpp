// Copyright Face Software. All Rights Reserved.


#include "Subsystems/LimenArchiveSubsystem.h"

#include "LimenStorageSaveData.h"
#include "Archives/LimenArchive.h"


bool ULimenArchiveSubsystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return true;
}

void ULimenArchiveSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	SaveDataName = TEXT("ArchivesData");
	Super::Initialize(Collection);
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
		verify(GetCurrentSaveData()->GetObjectSaveData(i, SaveData));
		
		// Create the archive
		const TSoftClassPtr<UObject>& ArchiveClassSoftPtr = SaveData.GetObjectClass();
		CurrentArchiveClass = ArchiveClassSoftPtr.LoadSynchronous();
		check(IsValid(CurrentArchiveClass));
		ULimenArchive* Archive = NewObject<ULimenArchive>(this, CurrentArchiveClass);
		check(IsValid(Archive));
		// Load data into the specific archive
		SaveData.LoadData(Archive);

		// Add the item again into the list and bind events (without saving the list!!)
		AddItem(Archive);
		Archive->OnArchiveRead.AddUniqueDynamic(this, &ThisClass::Save);
	}
}
