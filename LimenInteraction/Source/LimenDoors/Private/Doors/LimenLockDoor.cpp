// Copyright Face Software. All Rights Reserved.


#include "Doors/LimenLockDoor.h"

#include "Components/LimenKeyLock.h"

ALimenLockDoor::ALimenLockDoor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	Lock = CreateDefaultSubobject<ULimenKeyLock>(TEXT("KeyLock"));
}

void ALimenLockDoor::Interact(AController* InController, APawn* InPawn)
{
	if (Lock->Unlock(GetKeyFromPlayer(InController, InPawn)))
	{
		Super::Interact(InController, InPawn);
	}
}

ULimenKeyLock* ALimenLockDoor::GetLock() const
{
	return Lock.Get();
}
