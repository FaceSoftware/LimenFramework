// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Doors/LimenDoorBase.h"
#include "LimenKeyCardDoor.generated.h"

class ALimenKeyCard;
class ULimenKeyCardReader;

UCLASS()
class LIMENDOORS_API ALimenKeyCardDoor : public ALimenDoorBase
{
	GENERATED_BODY()

public:
	ALimenKeyCardDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	virtual void Interact(AController* InController, APawn* InPawn) override;

	UFUNCTION(BlueprintImplementableEvent)
	ALimenKeyCard* GetKeyCardFromPlayer(AController* InController, APawn* InPawn);

	UFUNCTION(BlueprintCallable)
	ULimenKeyCardReader* GetKeyCardReader() const;

private:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetKeyCardReader")
	TObjectPtr<ULimenKeyCardReader> KeyCardReader;
};
