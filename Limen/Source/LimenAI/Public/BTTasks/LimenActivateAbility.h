// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "LimenActivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class LIMENAI_API ULimenActivateAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	ULimenActivateAbility();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AbilityInstance;
};
