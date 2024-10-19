// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "OpenDoor.generated.h"

/**
 * 
 */
UCLASS()
class LIMENAI_API UOpenDoor : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UOpenDoor();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DoorActor;
};
