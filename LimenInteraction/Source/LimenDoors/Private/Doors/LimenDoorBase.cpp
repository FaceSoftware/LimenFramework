// Copyright Face Software. All Rights Reserved.


#include "Doors/LimenDoorBase.h"


ALimenDoorBase::ALimenDoorBase(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	bStartOpen = false;
	DoorState = EDoorState::Undefined;
}

void ALimenDoorBase::BeginPlay()
{
	Super::BeginPlay();

	if (bStartOpen)
	{
		OpenDoorWithoutAnimation();
	}
	else
	{
		CloseDoorWithoutAnimation();
	}
}

void ALimenDoorBase::SetStartOpen(const bool bNewStartOpen)
{
	bStartOpen = bNewStartOpen;
}

void ALimenDoorBase::OpenDoor(AController* InController, APawn* InPawn, const bool bIgnoreRestrictions)
{
	if (DoorState == EDoorState::Open || DoorState == EDoorState::Opening)
	{
		return;
	}

	DoorOpenAnimation();
	DoorState = EDoorState::Opening;
	
	OnDoorStateChanged.Broadcast(DoorState);
}

void ALimenDoorBase::OpenDoorWithoutAnimation(AController* InController, APawn* InPawn, const bool bIgnoreRestrictions)
{
	if (DoorState == EDoorState::Open)
	{
		return;
	}

	SetDoorOpenWithoutAnimation();
	DoorState = EDoorState::Open;
	
	OnDoorStateChanged.Broadcast(DoorState);
}

void ALimenDoorBase::CloseDoor(AController* InController, APawn* InPawn)
{
	if (DoorState == EDoorState::Closed || DoorState == EDoorState::Closing)
	{
		return;
	}
	
	DoorCloseAnimation();
	DoorState = EDoorState::Closing;
	
	OnDoorStateChanged.Broadcast(DoorState);
}

void ALimenDoorBase::CloseDoorWithoutAnimation(AController* InController, APawn* InPawn)
{
	if (DoorState == EDoorState::Closed)
	{
		return;
	}
	
	SetDoorCloseWithoutAnimation();
	DoorState = EDoorState::Closed;
	
	OnDoorStateChanged.Broadcast(DoorState);
}

bool ALimenDoorBase::IsOpen() const
{
	return DoorState == EDoorState::Open;
}

EDoorState ALimenDoorBase::GetDoorState() const
{
	return DoorState;
}

void ALimenDoorBase::Interact(AController* InController, APawn* InPawn)
{
	if (DoorState == EDoorState::Open || DoorState == EDoorState::Opening)
	{
		CloseDoor(InController, InPawn);
	}
	else if (DoorState == EDoorState::Closed || DoorState == EDoorState::Closing)
	{
		OpenDoor(InController, InPawn);
	}
}

void ALimenDoorBase::ToggleOpenState()
{
	if (DoorState == EDoorState::Open || DoorState == EDoorState::Opening)
	{
		CloseDoor();
	}
	else if (DoorState == EDoorState::Closed || DoorState == EDoorState::Closing)
	{
		OpenDoor();
	}
}

void ALimenDoorBase::NotifyAnimationFinished(const bool bIsOpenAnimation)
{
	DoorState = bIsOpenAnimation ? EDoorState::Open : EDoorState::Closed;
	
	TArray<UActorComponent*> PrimitiveComponents = GetComponentsByTag(UPrimitiveComponent::StaticClass(), DoorComponentTag);
	check(PrimitiveComponents.Num() <= 1);
	if (PrimitiveComponents.IsValidIndex(0))
	{
		PrimitiveComponents[0]->SetCanEverAffectNavigation(bIsOpenAnimation);
	}
	
	if (bIsOpenAnimation)
	{
	}
	else
	{
	}

	OnDoorStateChanged.Broadcast(DoorState);
}

void ALimenDoorBase::SetDoorState(const EDoorState NewState)
{
	DoorState = NewState;
}
