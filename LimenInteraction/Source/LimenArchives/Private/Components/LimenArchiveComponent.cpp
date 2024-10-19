// Copyright Face Software. All Rights Reserved.


#include "Components/LimenArchiveComponent.h"

#include "Archives/LimenArchive.h"
#include "Subsystems/LimenArchiveSubsystem.h"

ULimenArchiveComponent::ULimenArchiveComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool ULimenArchiveComponent::HasAlreadyBeenArchived() const
{
	const ULimenArchiveSubsystem* ArchivesManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenArchiveSubsystem>();
	const TSubclassOf<ULimenStorageItem> ArchiveClass = BoundArchiveClass.LoadSynchronous();
	return ArchivesManager->GetItem(ArchiveClass) != nullptr; 
}

ULimenArchive* ULimenArchiveComponent::GetArchive() const
{
	if (!BoundArchiveClass.IsNull())
	{
		ULimenArchiveSubsystem* ArchivesManager = GetWorld()->GetGameInstance()->GetSubsystem<ULimenArchiveSubsystem>();
		return NewObject<ULimenArchive>(ArchivesManager, BoundArchiveClass.LoadSynchronous());
	}

	return nullptr;
}
