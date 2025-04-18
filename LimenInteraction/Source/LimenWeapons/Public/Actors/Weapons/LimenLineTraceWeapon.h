// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenWeapon.h"
#include "LimenLineTraceWeapon.generated.h"

class ULimenDamageType;

UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ALimenLineTraceWeapon : public ALimenWeapon
{
	GENERATED_BODY()

public:
	ALimenLineTraceWeapon();
	virtual void PickUp(AController* InController, APawn* InPawn) override;

protected:
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Debug")
	bool bDebugMode;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen|Weapon Stats")
	TEnumAsByte<ECollisionChannel> TraceChannel;

	TWeakObjectPtr<APawn> CachedOwnerPawn;
	
	virtual void FireMethod() override;
};
