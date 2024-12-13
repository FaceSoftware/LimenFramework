// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GetRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class LIMENAI_API UGetRandomLocation : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UGetRandomLocation();
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
protected:

private:
	UPROPERTY(EditAnywhere)
	int32 MaxAttempts;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector OutLocation;
	UPROPERTY(EditAnywhere)
	float MaxRadius;
	UPROPERTY(EditAnywhere)
	float MinRadius;
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius;

	
};
