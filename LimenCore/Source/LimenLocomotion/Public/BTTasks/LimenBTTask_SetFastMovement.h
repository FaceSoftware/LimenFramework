// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "LimenBTTask_SetFastMovement.generated.h"


/**
 * 
 */
UCLASS()
class LIMENLOCOMOTION_API ULimenBTTask_SetFastMovement : public UBTTaskNode
{
	GENERATED_BODY()

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY(EditAnywhere)
	FValueOrBBKey_Bool Activate;

};
