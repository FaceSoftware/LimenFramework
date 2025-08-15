// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ReadAttributeValue.generated.h"


UENUM()
enum class EReadAttributeMethod
{
	// Literal value of the attribute from Min to Max
	Literal,
	// Attribute value in a 0 to 1 range.
	Fraction,
	// Attribute value in a 0 to 100 range.
	Percentage,
};

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API UBTService_ReadAttributeValue : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ReadAttributeValue();
	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector AttributeInstanceKey;
	UPROPERTY(EditAnywhere)
	FBlackboardKeySelector KeyToStoreValue;
	UPROPERTY(EditAnywhere)
	EReadAttributeMethod ReadValueMethod;
	
};
