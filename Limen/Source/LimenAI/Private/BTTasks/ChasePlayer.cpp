// Copyright Face Software. All Rights Reserved.


#include "BTTasks/ChasePlayer.h"

#include "AIController.h"
#include "Kismet/GameplayStatics.h"

UChasePlayer::UChasePlayer()
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const EBTNodeResult::Type ParentResult = Super::ExecuteTask(OwnerComp, NodeMemory);
		ParentResult != EBTNodeResult::Succeeded)
	{
		return ParentResult;
	}

	AAIController* Controller = OwnerComp.GetAIOwner();
	if (!Controller->IsValidLowLevelFast())
	{
		return EBTNodeResult::Type::Failed;
	}

	AActor* Player = UGameplayStatics::GetPlayerPawn(this, PlayerId);
	if (!Player->IsValidLowLevelFast())
	{
		return EBTNodeResult::Type::Failed;
	}
	
	Controller->MoveToActor(Player, 1.f, false, true, true, nullptr, true);

	if (FVector::DistSquared(Player->GetActorLocation(), OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation()) > FMath::Square(1.f))
	{
		return EBTNodeResult::Type::InProgress;
	}

	return EBTNodeResult::Type::Succeeded;
}
