// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenDoorBase.h"
#include "CppClasses/LimenNotification.h"
#include "LimenLockDoor.generated.h"


class ULimenLock;

UCLASS()
class LIMENDOORS_API ALimenLockDoor : public ALimenDoorBase
{
	GENERATED_BODY()

public:
	explicit ALimenLockDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;

	ULimenLock* GetInsideLock() const;
	ULimenLock* GetOutsideLock() const;

	UFUNCTION(BlueprintCallable)
	bool IsAnyHandleLocked() const;

protected:
	UPROPERTY(EditAnywhere, Category="Limen|Notifications")
	FNotificationParams DoorUnlockFailedNotification;
	UPROPERTY(EditAnywhere, Category="Limen|Notifications")
	FNotificationParams DoorUnlockSuccessNotification;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULimenLock> InsideLock;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<ULimenLock> OutsideLock;

	TWeakObjectPtr<ULimenInteractableComponent> InsideHandleInteraction;
	TWeakObjectPtr<ULimenInteractableComponent> OutsideHandleInteraction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Config")
	FName InsideHandleInteractionTag;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen|Config")
	FName OutsideHandleInteractionTag;

	virtual void Interact(AController* InController, APawn* InPawn) override;
	virtual void InteractInsideHandle(AController* InController, APawn* InPawn);
	virtual void InteractOutsideHandle(AController* InController, APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	virtual ALimenKey* GetKeyFromPlayer(ULimenLock* LockContext, AController* InController, APawn* InPawn) const;
	virtual void OnDoorTryUnlock(AController* InController, APawn* InPawn, bool bSuccess);

	void UnlockAllLocks(ALimenKey* Key, AController* InController, APawn* InPawn) const;

private:
	void UnlockFromInside(AController* InController, APawn* InPawn);
	void UnlockFromOutside(AController* InController, APawn* InPawn);
};
