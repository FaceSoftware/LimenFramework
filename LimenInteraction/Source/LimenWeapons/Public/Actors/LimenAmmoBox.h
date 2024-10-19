// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenAmmo.h"
#include "LimenAmmoBox.generated.h"


UCLASS(Abstract)
class LIMENWEAPONS_API ALimenAmmoBox : public ALimenGameplayActor
{
	GENERATED_BODY()

public:
	ALimenAmmoBox();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="1"))
	int32 AmmoCount;
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="1"))
	TSubclassOf<ALimenAmmo> AmmoClass;
	UPROPERTY()
	TArray<ALimenAmmo*> AmmoInstances;

	virtual void Interact(AController* InController, APawn* InPawn);
};
