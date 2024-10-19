// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenWeapon.h"
#include "LimenLineTraceWeapon.generated.h"

UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ALimenLineTraceWeapon : public ALimenWeapon
{
	GENERATED_BODY()

public:
	ALimenLineTraceWeapon();

protected:
	
private:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Debug")
	bool bDebugMode;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen|Weapon Stats")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	virtual void FireMethod() override;
};
