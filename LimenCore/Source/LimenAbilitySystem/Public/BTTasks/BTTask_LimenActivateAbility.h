// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_LimenActivateAbility.generated.h"

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API UBTTask_LimenActivateAbility : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_LimenActivateAbility();
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AbilityInstance;
};
