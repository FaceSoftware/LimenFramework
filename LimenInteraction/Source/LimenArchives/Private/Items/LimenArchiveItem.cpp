// Copyright Face Software. All Rights Reserved.


#include "Items/LimenArchiveItem.h"

#include "Components/LimenArchiveComponent.h"
#include "Subsystems/LimenArchiveSubsystem.h"


ALimenArchiveItem::ALimenArchiveItem(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ArchiveComponent = CreateDefaultSubobject<ULimenArchiveComponent>(TEXT("ArchiveComponent"));
}

void ALimenArchiveItem::BeginPlay()
{
	Super::BeginPlay();

	if (ArchiveComponent->HasAlreadyBeenArchived())
	{
		RemoveFromGameplay();
	}
}

void ALimenArchiveItem::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);

	ULimenArchive* Archive = ArchiveComponent->GetArchive();
	if (Archive == nullptr)
	{
		return;
	}

	ULimenArchiveSubsystem* ArchivesManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenArchiveSubsystem>();
	check(Archive != nullptr);
	ArchivesManager->AddArchive(Archive);
	RemoveFromGameplay();
}
