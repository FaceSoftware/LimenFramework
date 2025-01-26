// Copyright Face Software. All Rights Reserved.


#include "AIControllers/LimenAIControllerBase.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"


ALimenAIControllerBase::ALimenAIControllerBase()
{
	PrimaryActorTick.bCanEverTick = true;

	bStartAILogicOnPossess = true;
	bStopAILogicOnUnposses = true;

	BehaviorTreeComponent = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourTreeComponent"));
	BlackboardComponent = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComponent"));
	AIPerception = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));
}

void ALimenAIControllerBase::BeginPlay()
{
	Super::BeginPlay();

	AIPerception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::BP_OnAIPerceptionUpdated);
	AIPerception->OnTargetPerceptionUpdated.AddUniqueDynamic(this, &ThisClass::OnAIPerceptionUpdated);
	AIPerception->OnTargetPerceptionInfoUpdated.AddUniqueDynamic(this, &ThisClass::BP_OnAIPerceptionInfoUpdated);
	AIPerception->OnTargetPerceptionInfoUpdated.AddUniqueDynamic(this, &ThisClass::OnAIPerceptionInfoUpdated);
}

void ALimenAIControllerBase::OnAIPerceptionUpdated(AActor* StimulusSource, const FAIStimulus Stimulus)
{
	if (!Stimulus.IsValid())
	{
		return;
	}

	bIsSensing = Stimulus.WasSuccessfullySensed();
	StimulusLocation = Stimulus.StimulusLocation;
	LastSensedActor = StimulusSource;
}

void ALimenAIControllerBase::OnAIPerceptionInfoUpdated(const FActorPerceptionUpdateInfo& UpdateInfo)
{
	LastSensedActor = UpdateInfo.Target;
	bIsSensing = UpdateInfo.Stimulus.WasSuccessfullySensed();
	StimulusLocation = UpdateInfo.Stimulus.StimulusLocation;
}

bool ALimenAIControllerBase::IsSensing() const
{
	return bIsSensing;
}

FVector ALimenAIControllerBase::GetLastStimulusLocation() const
{
	return StimulusLocation;
}

AActor* ALimenAIControllerBase::GetLastSensedActor() const
{
	return LastSensedActor.Get();
}

UBehaviorTree* ALimenAIControllerBase::GetBehaviourTree() const
{
	return BehaviorTreeComponent->GetCurrentTree();
}

UBlackboardData* ALimenAIControllerBase::GetBlackboard() const
{
	return BehaviorTreeComponent->GetBlackboardComponent()->GetBlackboardAsset();
}

void ALimenAIControllerBase::ResumeBrainLogic(const FString& Reason)
{
	if (bIsBrainLogicActive || GetBrainComponent() == nullptr)
	{
		return;	
	}

	GetBrainComponent()->ResumeLogic(Reason);
	bIsBrainLogicActive = true;
}

void ALimenAIControllerBase::PauseBrainLogic(const FString& Reason)
{
	if (!bIsBrainLogicActive || GetBrainComponent() == nullptr)
	{
		return;	
	}

	GetBrainComponent()->PauseLogic(Reason);
	bIsBrainLogicActive = false;
}
