// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenDamageType.generated.h"

class UActorComponent;


USTRUCT(BlueprintType)
struct FDamageParameters
{
	GENERATED_BODY()

	FDamageParameters() = default;

	UPROPERTY(BlueprintReadOnly)
	float DamageValue = 0;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<UPrimitiveComponent> HitComponent;
	UPROPERTY(BlueprintReadOnly)
	FName HitBoneName;
	UPROPERTY(BlueprintReadOnly)
	FRotator DamageDirection = FRotator::ZeroRotator;
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
