// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LimenAIControllerBase.generated.h"


class UBehaviorTreeComponent;
class UBlackboardComponent;
struct FActorPerceptionUpdateInfo;
struct FAIStimulus;
class UAIPerceptionComponent;

UCLASS()
class LIMENGAMEFRAMEWORK_API ALimenAIControllerBase : public AAIController
{
	GENERATED_BODY()

public:
	ALimenAIControllerBase();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ResumeBrainLogic(const FString& Reason);
	UFUNCTION(BlueprintCallable)
	void PauseBrainLogic(const FString& Reason);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBlackboardComponent> BlackboardComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAIPerceptionComponent> AIPerception;
	
	UFUNCTION(BlueprintImplementableEvent, Category="Limen|AI", DisplayName="On AI Perception Updated")
	void BP_OnAIPerceptionUpdated(AActor* StimulusSource, const FAIStimulus Stimulus);
	UFUNCTION()
	virtual void OnAIPerceptionUpdated(AActor* StimulusSource, const FAIStimulus Stimulus);

	UFUNCTION(BlueprintImplementableEvent, Category="Limen|AI", DisplayName="On AI Perception Info Updated")
	void BP_OnAIPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);
	UFUNCTION()
	virtual void OnAIPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo);
	
	bool IsSensing() const;
	FVector GetLastStimulusLocation() const;
	AActor* GetLastSensedActor() const;

	UFUNCTION(BlueprintCallable)
	UBehaviorTree* GetBehaviourTree() const;
	UFUNCTION(BlueprintCallable)
	UBlackboardData* GetBlackboard() const;

private:
	bool bIsSensing;
	FVector StimulusLocation;
	TWeakObjectPtr<AActor> LastSensedActor;
	bool bIsBrainLogicActive;
};
