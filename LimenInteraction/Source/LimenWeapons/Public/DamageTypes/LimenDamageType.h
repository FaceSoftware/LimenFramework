// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenDamageType.generated.h"

class UActorComponent;


USTRUCT()
struct FDamageParameters
{
	GENERATED_BODY()

	FDamageParameters() = default;

	float DamageValue;
	TWeakObjectPtr<UPrimitiveComponent> HitComponent;
	FName HitBoneName;
};

UCLASS()
class LIMENWEAPONS_API ULimenDamageType : public UObject
{
	GENERATED_BODY()

public:
	ULimenDamageType();

	virtual float ProcessRawDamage(const float DeltaSeconds, const FDamageParameters& DamageParams);
	virtual bool ShouldStopApplyingDamage() const;

protected:

private:
};
