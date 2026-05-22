// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "MoveAwayFromActor.generated.h"

/**
 * 
 */
UCLASS()
class LIMENAI_API UMoveAwayFromActor : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UMoveAwayFromActor();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

private:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector TargetActor;
	UPROPERTY()
	TObjectPtr<AActor> Actor;

	UPROPERTY(EditAnywhere)
	float Radius;
	
};
