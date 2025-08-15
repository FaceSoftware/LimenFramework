// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ReadAbilityState.generated.h"

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API UBTService_ReadAbilityState : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ReadAbilityState();
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector KeyToStoreValue;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AbilityInstanceKey;
};
