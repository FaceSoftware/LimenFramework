// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTDecorator_RandomChance.generated.h"


struct FValueOrBBKey_Float;
/**
 * @brief Generates a random value in a [0, 1] range.
 * Succeeds if the generated random value is below or equal to ChanceToSucceedBlackboardKey, fails otherwise.
 */
UCLASS()
class LIMENGLOBALRANDOMSTREAM_API UBTDecorator_RandomChance : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_RandomChance();
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

protected:
	UPROPERTY(EditAnywhere)
	FValueOrBBKey_Float ChanceToSucceedBlackboardKey;
	/**
	 * @brief Will generate a random value between MaxRandomValueRange and MaxRandomValueRange
	 */
	UPROPERTY(EditAnywhere)
	float MinRandomValueRange;
	/**
	 * @brief Will generate a random value between MaxRandomValueRange and MaxRandomValueRange
	 */
	UPROPERTY(EditAnywhere)
	float MaxRandomValueRange;
};
