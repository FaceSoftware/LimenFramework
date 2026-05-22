// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetActorLocation.generated.h"

/**
 * 
 */
UCLASS()
class LIMENAI_API UBTService_GetActorLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetActorLocation();
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector OutLocation;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector Target;

private:
};
