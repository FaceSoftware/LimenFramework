// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "LimenArchiveItem.generated.h"

class ULimenArchiveComponent;

UCLASS()
class LIMENARCHIVES_API ALimenArchiveItem : public ALimenInteractable
{
	GENERATED_BODY()

public:
	explicit ALimenArchiveItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

protected:
	virtual void Interact(AController* InController, APawn* InPawn) override;

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TObjectPtr<ULimenArchiveComponent> ArchiveComponent;
	
};
