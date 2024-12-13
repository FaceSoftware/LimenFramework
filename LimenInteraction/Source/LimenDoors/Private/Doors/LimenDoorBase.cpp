// Copyright Face Software. All Rights Reserved.


#include "Doors/LimenDoorBase.h"


ALimenDoorBase::ALimenDoorBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	bStartOpen = false;
	bIsOpen = false;
	bUseAnimationOnStart = true;
}

void ALimenDoorBase::BeginPlay()
{
	Super::BeginPlay();

	if (bStartOpen)
	{
		bUseAnimationOnStart ? OpenDoor() : OpenDoorWithoutAnimation();
	}
	else
	{
		bUseAnimationOnStart ? CloseDoor() : CloseDoorWithoutAnimation();
	}
}

void ALimenDoorBase::SetStartOpen(const bool bNewStartOpen)
{
	bStartOpen = bNewStartOpen;
}

void ALimenDoorBase::OpenDoor(AController* InController, APawn* InPawn, const bool bIgnoreRestrictions)
{
	if (bIsOpen)
	{
		return;
	}

	DoorOpenAnimation();
	bIsOpen = true;
	DoorStateChanged(bIsOpen);
}

void ALimenDoorBase::OpenDoorWithoutAnimation(AController* InController, APawn* InPawn, const bool bIgnoreRestrictions)
{
	if (bIsOpen)
	{
		return;
	}
	
	if (!bIgnoreRestrictions)
	{

		ToggleOpenState();
	}

	SetDoorOpenWithoutAnimation();
	bIsOpen = true;
	DoorStateChanged(bIsOpen);
}

void ALimenDoorBase::CloseDoor(AController* InController, APawn* InPawn)
{
	if (!bIsOpen)
	{
		return;
	}
	
	DoorCloseAnimation();
	bIsOpen = false;
	DoorStateChanged(bIsOpen);
}

void ALimenDoorBase::CloseDoorWithoutAnimation(AController* InController, APawn* InPawn)
{
	if (!bIsOpen)
	{
		return;
	}
	
	SetDoorCloseWithoutAnimation();
	bIsOpen = false;
	DoorStateChanged(bIsOpen);
}

bool ALimenDoorBase::IsOpen() const
{
	return bIsOpen;
}

void ALimenDoorBase::Interact(AController* InController, APawn* InPawn)
{
	if (bIsOpen)
	{
		CloseDoor();
	}
	else
	{
		OpenDoor(InController, InPawn);
	}
}

void ALimenDoorBase::ToggleOpenState()
{
	if (IsOpen())
	{
		CloseDoor();
	}
	else
	{
		OpenDoor();
	}
}

void ALimenDoorBase::DoorStateChanged(const bool bDoorIsOpen)
{
	TArray<UActorComponent*> PrimitiveComponents = GetComponentsByTag(UPrimitiveComponent::StaticClass(), DoorComponentTag);
	check(PrimitiveComponents.Num() <= 1);
	if (PrimitiveComponents.IsValidIndex(0))
	{
		PrimitiveComponents[0]->SetCanEverAffectNavigation(bDoorIsOpen);
	}
	
	if (bIsOpen)
	{
	}
	else
	{
	}
}
