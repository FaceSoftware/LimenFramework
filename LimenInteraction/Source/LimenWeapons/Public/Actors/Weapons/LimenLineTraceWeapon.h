// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenWeapon.h"
#include "LimenLineTraceWeapon.generated.h"

UCLASS(Blueprintable, BlueprintType)
class LIMENWEAPONS_API ALimenLineTraceWeapon : public ALimenWeapon
{
	GENERATED_BODY()

	DECLARE_MULTICAST_DELEGATE_ThreeParams(FWeaponFiredDelegate, AController* /* Instigator */, AActor* /* Causer */, const TArray<FHitResult>& /* Hits */);

public:
	FWeaponFiredDelegate OnWeaponFired;
	
	explicit ALimenLineTraceWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

protected:
	
private:	
	UPROPERTY(EditDefaultsOnly, Category="Limen|Debug")
	bool bDebugMode;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen|Weapon Stats")
	TEnumAsByte<ECollisionChannel> TraceChannel;
	
	virtual void FireMethod() override;
};
