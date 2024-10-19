// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "LimenRecurrentAction.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FActionUpdate);
DECLARE_DYNAMIC_DELEGATE(FRecurrentActionDelegate);
DECLARE_DYNAMIC_DELEGATE_RetVal(bool, FRecurrentActionStopCondition);

/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenRecurrentAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	ULimenRecurrentAction();
	virtual void BeginDestroy() override;
	

	UPROPERTY(BlueprintAssignable)
	FActionUpdate OnSuccess;
	
	UPROPERTY(BlueprintAssignable)
	FActionUpdate OnFailure;
	
	/** Called to trigger the action once the delegates have been bound */
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, meta=(BlueprintInternalUseOnly="true", WorldContext="WorldContextObject"))
	static ULimenRecurrentAction* StartRecurrentAction(UObject* WorldContextObject, const FRecurrentActionDelegate& InRecurrentAction, const int32 InMaxRetries, const
													   float InTimeBetweenAttempts, const FRecurrentActionStopCondition& InStopCondition);


	static ULimenRecurrentAction* StartRecurrentAction(UWorld* Outer, const FRecurrentActionDelegate& InRecurrentAction, const int32 InMaxRetries, const
												   float InTimeBetweenAttempts, const FRecurrentActionStopCondition& InStopCondition);

protected:
	UPROPERTY()
	TObjectPtr<UWorld> World;
	
	FRecurrentActionDelegate RecurrentAction;
	FRecurrentActionStopCondition StopCondition;
	int32 MaxRetries;
	int32 CurrentRetries;
	float TimeBetweenAttempts;

private:
	float TimePassed;
	float TotalTimePassed;
	const float Timeout = 10.f;
	
	void Action_Internal();

	void WorldTickStart(UWorld* InWorld, ELevelTick LevelTick, float DeltaSeconds);
};
