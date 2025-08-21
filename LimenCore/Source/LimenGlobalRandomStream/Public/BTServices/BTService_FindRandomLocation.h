// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_FindRandomLocation.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGLOBALRANDOMSTREAM_API UBTService_FindRandomLocation : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_FindRandomLocation();
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	int32 MaxAttempts;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector OutLocation;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector SearchOrigin;
	UPROPERTY(EditAnywhere)
	float MaxRadius;
	UPROPERTY(EditAnywhere)
	float MinRadius;
	UPROPERTY(EditAnywhere)
	float AcceptanceRadius;
};
