// Copyright Face Software. All Rights Reserved.


#include "BTTasks/GetRandomLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"


UGetRandomLocation::UGetRandomLocation()
{
	MaxAttempts = 16;
	MaxRadius = 10000;
	MinRadius = 5000;
	AcceptanceRadius = 100;
}

EBTNodeResult::Type UGetRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (Super::ExecuteTask(OwnerComp, NodeMemory) == EBTNodeResult::Failed)
	{
		return EBTNodeResult::Failed;
	}

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (NavData == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	AAIController* OwnerController = CastChecked<AAIController>(OwnerComp.GetOwner());
	const APawn* ControllerPawn = OwnerController->GetPawn();

	FNavLocation Location;
	bool bLocationFound = false;
	for (int i = 0; i < MaxAttempts; ++i)
	{
		NavData->GetRandomReachablePointInRadius(ControllerPawn->GetActorLocation(), MaxRadius, Location);
		if (FVector::DistSquared(ControllerPawn->GetActorLocation(), Location.Location) >= FMath::Square(MinRadius))
		{
			bLocationFound = true;
			break;
		}
	}

	if (!bLocationFound)
	{
		return EBTNodeResult::Failed;
	}

	if (UBlackboardComponent* BlackboardComp = OwnerController->GetBlackboardComponent())
	{
		BlackboardComp->SetValueAsVector(OutLocation.SelectedKeyName, Location.Location);
	}
	
	return EBTNodeResult::Succeeded;
}
