// Copyright Face Software. All Rights Reserved.


#include "Items/LimenArchiveItem.h"

#include "Archives/LimenArchive.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "Subsystems/LimenArchiveSubsystem.h"


ALimenArchiveItem::ALimenArchiveItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ALimenArchiveItem::BeginPlay()
{
	Super::BeginPlay();

	if (HasAlreadyBeenArchived())
	{
		RemoveFromGameplay();
	}
}

bool ALimenArchiveItem::HasAlreadyBeenArchived() const
{
	const ULimenArchiveSubsystem* ArchivesManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenArchiveSubsystem>();
	const TSubclassOf<ULimenStorageItem> ArchiveClass = BoundArchiveClass.LoadSynchronous();
	return ArchivesManager->GetItem(ArchiveClass) != nullptr; 
}

TStrongObjectPtr<ULimenArchive> ALimenArchiveItem::GetArchive() const
{
	if (BoundArchiveClass.IsNull())
	{
		return TStrongObjectPtr<ULimenArchive>(nullptr);
	}
	
	if (!ArchivePtr.IsValid())
	{
		ULimenArchive* TempPtr = NewObject<ULimenArchive>(nullptr, BoundArchiveClass.LoadSynchronous());
		ArchivePtr = TStrongObjectPtr(TempPtr);
	}

	return ArchivePtr;
}


void ALimenArchiveItem::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);

	const TStrongObjectPtr<ULimenArchive> Archive = GetArchive();
	if (!Archive.IsValid())
	{
		return;
	}

	ULimenArchiveSubsystem* ArchivesManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenArchiveSubsystem>();
	check(ArchivesManager != nullptr)
	ArchivesManager->AddArchive(Archive.Get());
	RemoveFromGameplay();
	InteractionStopped(InController, InPawn);
}
