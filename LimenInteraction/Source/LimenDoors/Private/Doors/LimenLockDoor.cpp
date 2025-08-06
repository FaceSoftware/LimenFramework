// Copyright Face Software. All Rights Reserved.


#include "Doors/LimenLockDoor.h"

#include "Components/LimenInventoryComponent.h"
#include "Components/LimenLock.h"
#include "Components/LimenNotificationComponent.h"
#include "CppClasses/LimenNotification.h"
#include "GameFramework/HUD.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Keys/LimenKey.h"


ALimenLockDoor::ALimenLockDoor(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InsideLock = CreateDefaultSubobject<ULimenLock>(TEXT("InsideLock"));
	InsideLock->bAutoActivate = true;
	OutsideLock = CreateDefaultSubobject<ULimenLock>(TEXT("OutsideLock"));
	OutsideLock->bAutoActivate = true;
	
	DoorUnlockSuccessNotification.DisplayTime = 3.f;
	DoorUnlockSuccessNotification.NotificationTitle = FText::FromString(TEXT("Door"));
	DoorUnlockSuccessNotification.NotificationMessage = FText::FromString(TEXT("Door unlocked"));
	
	DoorUnlockFailedNotification.DisplayTime = 3.f;
	DoorUnlockFailedNotification.NotificationTitle = FText::FromString(TEXT("Door"));
	DoorUnlockFailedNotification.NotificationMessage = FText::FromString(TEXT("Door locked"));

	InsideHandleInteractionTag = TEXT("in");
	OutsideHandleInteractionTag = TEXT("out");
}

void ALimenLockDoor::BeginPlay()
{
	Super::BeginPlay();

	for (ILimenInteractableComponent* Component : GetInteractableComponents())
	{
		UActorComponent* ActorComponent = Cast<UActorComponent>(Component);
		check(ActorComponent != nullptr);

		if (ActorComponent->ComponentHasTag(InsideHandleInteractionTag))
		{
			InsideHandleInteraction = ActorComponent;
			InsideHandleInteraction->GetInteractionDelegate()->AddUObject(this, &ThisClass::InteractInsideHandle);
		}
		else if (ActorComponent->ComponentHasTag(OutsideHandleInteractionTag))
		{
			OutsideHandleInteraction = ActorComponent;
			OutsideHandleInteraction->GetInteractionDelegate()->AddUObject(this, &ThisClass::InteractOutsideHandle);
		}
	}
}

ULimenLock* ALimenLockDoor::GetInsideLock() const
{
	return InsideLock.Get();
}

ULimenLock* ALimenLockDoor::GetOutsideLock() const
{
	return OutsideLock.Get();
}

bool ALimenLockDoor::IsAnyHandleLocked() const
{
	return InsideLock->IsLocked() || OutsideLock->IsLocked();
}

void ALimenLockDoor::Interact(AController* InController, APawn* InPawn)
{
}

void ALimenLockDoor::InteractInsideHandle(AController* InController, APawn* InPawn)
{
	if (!InsideLock->IsLocked())
	{
		Super::Interact(InController, InPawn);
	}

	if (IsAnyHandleLocked())
	{
		UnlockFromInside(InController, InPawn);
		OnDoorTryUnlock(InController, InPawn, !IsAnyHandleLocked());
	}
}

void ALimenLockDoor::InteractOutsideHandle(AController* InController, APawn* InPawn)
{
	if (!OutsideLock->IsLocked())
	{
		Super::Interact(InController, InPawn);
		return;
	}
	
	if (IsAnyHandleLocked())
	{
		UnlockFromOutside(InController, InPawn);
		OnDoorTryUnlock(InController, InPawn, !IsAnyHandleLocked());
	}
}

ALimenKey* ALimenLockDoor::GetKeyFromPlayer(ULimenLock* LockContext, AController* InController, APawn* InPawn) const
{
	const ULimenInventoryComponent* Inventory = InPawn->GetComponentByClass<ULimenInventoryComponent>();
	if (!Inventory) return nullptr;
	
	for (ALimenKey* Key : Inventory->PeekItemInstances<ALimenKey>())
	{
		if (LockContext->TryKeyword(Key->GetKeyword()))
		{
			return Key;
		}
	}

	return nullptr;
}

void ALimenLockDoor::OnDoorTryUnlock(AController* InController, APawn* InPawn, const bool bSuccess)
{
	const APlayerController* PlayerController = Cast<APlayerController>(InController);
	if (PlayerController == nullptr)
	{
		return;
	}

	const AHUD* PlayerHud = PlayerController->GetHUD();
	if (PlayerHud == nullptr)
	{
		return;
	}

	ULimenNotificationComponent* NotificationComponent = PlayerHud->GetComponentByClass<ULimenNotificationComponent>();
	if (NotificationComponent == nullptr)
	{
		return;
	}

	FNotificationParams& Params = bSuccess ? DoorUnlockSuccessNotification : DoorUnlockFailedNotification;
	const TSharedRef<FLimenNotification> Notification = MakeShared<FLimenNotification>(Params);
	NotificationComponent->QueueNotification(Notification);
}

void ALimenLockDoor::UnlockAllLocks(ALimenKey* Key, AController* InController, APawn* InPawn) const
{
}

void ALimenLockDoor::UnlockFromInside(AController* InController, APawn* InPawn)
{
	ALimenKey* Key = GetKeyFromPlayer(OutsideLock.Get(), InController, InPawn);
	if (InsideLock->ChangeLockedState(InController, InPawn, Key->GetKeyword(), false))
	{
		OutsideLock->SetLockedState(false);
	}
}

void ALimenLockDoor::UnlockFromOutside(AController* InController, APawn* InPawn)
{
	ALimenKey* Key = GetKeyFromPlayer(OutsideLock.Get(), InController, InPawn);
	if (OutsideLock->ChangeLockedState(InController, InPawn, Key->GetKeyword(), false))
	{
		InsideLock->SetLockedState(false);
	}
}
