// Copyright Face Software. All Rights Reserved.


#include "BTTasks/FindClosestDoor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BlueprintLibraries/LimenMath.h"
#include "Components/Interaction/LimenInteractionComponent.h"
#include "Components/Interaction/LimenProximityInteractionComponent.h"
#include "Doors/LimenDoorBase.h"

EBTNodeResult::Type UFindClosestDoor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const EBTNodeResult::Type ParentResult = Super::ExecuteTask(OwnerComp, NodeMemory);
		ParentResult != EBTNodeResult::Succeeded)
	{
		return ParentResult;
	}

	UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UActorComponent* Component = OwnerComp.GetAIOwner()->GetPawn()->GetComponentByClass(ULimenInteractionComponent::StaticClass());
	if (Component == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	const auto* InteractionComponent = Cast<ULimenProximityInteractionComponent>(Component);
	if (!IsValid(InteractionComponent))
	{
		return EBTNodeResult::Failed;
	}
	
	AActor* TempClosestDoor = ULimenMath::GetClosestActorOfClassTo(OwnerComp.GetAIOwner()->GetPawn()->GetActorLocation(), InteractionComponent->GetAllInteractableActorsInRange(), ALimenDoorBase::StaticClass());
	ALimenDoorBase* ClosestDoor = Cast<ALimenDoorBase>(TempClosestDoor);
	
	BlackboardComponent->SetValueAsObject(DoorActor.SelectedKeyName, ClosestDoor);
	return ClosestDoor != nullptr ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
