// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenAmmo.h"
#include "LimenAmmoBox.generated.h"


struct FActorSpawnParameters;

UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ALimenAmmoBox : public ALimenAmmo
{
	GENERATED_BODY()

public:
	ALimenAmmoBox();

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="1"))
	int32 AmmoCount;
	UPROPERTY(EditDefaultsOnly, Category="Limen", meta=(ClampMin="1"))
	TSubclassOf<ALimenAmmo> AmmoClass;

	virtual void Interact(AController* InController, APawn* InPawn) override;

private:
};
