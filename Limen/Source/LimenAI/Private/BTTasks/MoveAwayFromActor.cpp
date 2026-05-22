// Copyright Face Software. All Rights Reserved.


#include "BTTasks/MoveAwayFromActor.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Navigation/PathFollowingComponent.h"


UMoveAwayFromActor::UMoveAwayFromActor()
{
	Radius = 5000;
}

EBTNodeResult::Type UMoveAwayFromActor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* OwnerController = CastChecked<AAIController>(OwnerComp.GetOwner());
	const APawn* ControllerPawn = OwnerController->GetPawn();

	if (const UBlackboardComponent* BlackboardComp = OwnerController->GetBlackboardComponent())
	{
		Actor = Cast<AActor>(BlackboardComp->GetValueAsObject(TargetActor.SelectedKeyName));
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
	
	// Calculate direction vector from the actor to the AI pawn
	FVector DirectionAway = ControllerPawn->GetActorLocation() - Actor->GetActorLocation();
	DirectionAway.Normalize(); // Normalize to get direction only

	// Scale the direction to your desired distance
	FVector TargetLocation = ControllerPawn->GetActorLocation() + DirectionAway * Radius;

	FNavLocation NavLocation;
	if(!NavSys->GetRandomPointInNavigableRadius(TargetLocation, Radius, NavLocation))
	{
		return EBTNodeResult::Failed;
	}
	
	TargetLocation = NavLocation.Location;

	const EPathFollowingRequestResult::Type Result = OwnerController->MoveToLocation(NavLocation.Location, Radius);
	check(Result != EPathFollowingRequestResult::Type::Failed);
	
	return EBTNodeResult::Succeeded;
}
