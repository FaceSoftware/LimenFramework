// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LimenDeactivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API UBTTask_LimenDeactivateAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_LimenDeactivateAbility();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AbilityInstance;
};
