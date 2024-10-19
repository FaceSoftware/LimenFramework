// Copyright Face Software. All Rights Reserved.


#include "Doors/LimenKeyCardDoor.h"

#include "Components/LimenKeyCardReader.h"


ALimenKeyCardDoor::ALimenKeyCardDoor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	KeyCardReader = CreateDefaultSubobject<ULimenKeyCardReader>(TEXT("KeyCardReader"));
}

void ALimenKeyCardDoor::Interact(AController* InController, APawn* InPawn)
{
	if (KeyCardReader->ReadKeyCard(GetKeyCardFromPlayer(InController, InPawn)))
	{
		Super::Interact(InController, InPawn);
	}
}

ULimenKeyCardReader* ALimenKeyCardDoor::GetKeyCardReader() const
{
	return KeyCardReader.Get();
}
