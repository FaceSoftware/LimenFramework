// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/ValueOrBBKey.h"
#include "Runtime/AIModule/Classes/BehaviorTree/BTService.h"
#include "BTService_Timer.generated.h"

/**
 * 
 */
UCLASS()
class LIMENCORE_API UBTService_Timer : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_Timer();
    virtual uint16 GetInstanceMemorySize() const override { return sizeof(FTimerMem); }
    virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
    virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;\
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
    virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	

protected:
	UPROPERTY(EditAnywhere, Category="Timer")
	FValueOrBBKey_Float Duration;
	UPROPERTY(EditAnywhere, Category="Timer")
	bool bLoop;
	UPROPERTY(EditAnywhere, Category="Blackboard")
	FBlackboardKeySelector DoneKey;

	struct FTimerMem { float Deadline = -1.f; bool bFired = false; };

private:
};
