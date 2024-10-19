// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "ChasePlayer.generated.h"

/**
 * 
 */
UCLASS()
class LIMENAI_API UChasePlayer : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UChasePlayer();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere)
	uint32 PlayerId = 0;
};
