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

void ALimenArchiveItem::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);

	if (BoundArchiveClass.IsNull())
	{
		return;
	}

	ULimenArchiveSubsystem* ArchivesManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenArchiveSubsystem>();
	check(ArchivesManager != nullptr)
	
	if (!ArchivePtr.IsValid())
	{
		ArchivePtr = NewObject<ULimenArchive>(ArchivesManager, BoundArchiveClass.LoadSynchronous());
	}

	ArchivesManager->AddArchive(ArchivePtr.Get());
	RemoveFromGameplay();
	InteractionStopped(InController, InPawn);
}
