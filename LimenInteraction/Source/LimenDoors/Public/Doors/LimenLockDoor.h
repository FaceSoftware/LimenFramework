// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenDoorBase.h"
#include "LimenLockDoor.generated.h"

class ULimenKeyLock;

UCLASS()
class LIMENDOORS_API ALimenLockDoor : public ALimenDoorBase
{
	GENERATED_BODY()

public:
	ALimenLockDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Interact(AController* InController, APawn* InPawn) override;

	UFUNCTION(BlueprintImplementableEvent)
	ALimenKey* GetKeyFromPlayer(AController* InController, APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	ULimenKeyLock* GetLock() const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetLock")
	TObjectPtr<ULimenKeyLock> Lock;
};
