// Copyright Face Software. All Rights Reserved.


#include "BTTasks/OpenDoor.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Object.h"
#include "Doors/LimenDoorBase.h"

UOpenDoor::UOpenDoor()
{
	NodeName = TEXT("Open Door");
	DoorActor.AllowedTypes.Push(NewObject<UBlackboardKeyType_Object>());
}

EBTNodeResult::Type UOpenDoor::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (const EBTNodeResult::Type ParentResult = Super::ExecuteTask(OwnerComp, NodeMemory);
		ParentResult != EBTNodeResult::Succeeded)
	{
		return ParentResult;
	}

	const UBlackboardComponent* BlackboardComponent = OwnerComp.GetBlackboardComponent();
	if (BlackboardComponent == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	UObject* TempDoorPtr = BlackboardComponent->GetValueAsObject(DoorActor.SelectedKeyName);
	if (TempDoorPtr == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	ALimenDoorBase* DoorPtr = Cast<ALimenDoorBase>(TempDoorPtr);
	if (DoorPtr == nullptr)
	{
		return EBTNodeResult::Failed;
	}

	if (DoorPtr->IsOpen())
	{
		return EBTNodeResult::Failed;
	}

	DoorPtr->OpenDoor(OwnerComp.GetAIOwner(), OwnerComp.GetAIOwner()->GetPawn(), false);
	return EBTNodeResult::Succeeded;
}
