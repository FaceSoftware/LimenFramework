// Copyright FaceSoftware. All Rights Reserved.


#include "BTTasks/LimenBTTask_SetFastMovement.h"

#include "AIController.h"
#include "Components/LimenMovementComponent.h"


EBTNodeResult::Type ULimenBTTask_SetFastMovement::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type ParentResult = Super::ExecuteTask(OwnerComp, NodeMemory);
	if (ParentResult != EBTNodeResult::Succeeded) return ParentResult;

	const auto AIController = OwnerComp.GetOwner<AAIController>();
	if (!AIController) return EBTNodeResult::Failed;

	const APawn* Pawn = AIController->GetPawn();
	if (!Pawn) return EBTNodeResult::Failed;

	auto* MovementComponent = Pawn->GetComponentByClass<ULimenMovementComponent>();
	if (!MovementComponent) return EBTNodeResult::Failed;

	MovementComponent->SetFastMovement(Activate.GetValue(OwnerComp));
	return EBTNodeResult::Succeeded;
}
