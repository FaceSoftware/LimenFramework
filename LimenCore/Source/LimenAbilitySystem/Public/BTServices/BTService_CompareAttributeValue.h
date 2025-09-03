// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "BTService_CompareAttributeValue.generated.h"


enum class EComparisonOperator : uint8;

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API UBTService_CompareAttributeValue : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_CompareAttributeValue();
	virtual void OnSearchStart(FBehaviorTreeSearchData& SearchData) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttributeInstance;
	UPROPERTY(EditAnywhere)
	FValueOrBBKey_Float ValueToCompareAgainst;
	UPROPERTY(EditAnywhere)
	EComparisonOperator ComparisonOperator;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector ComparisonResult;
};
