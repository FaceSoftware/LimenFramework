// Copyright © 2024 FaceSoftware. All rights reserved.


#include "BTTasks/BTTask_LimenActivateAbility.h"

#include "Abilities/LimenActiveAbility.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerController.h"


UBTTask_LimenActivateAbility::UBTTask_LimenActivateAbility()
{
	NodeName = TEXT("Activate Limen Ability");
	AbilityInstance.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LimenActivateAbility, AbilityInstance), ULimenActiveAbility::StaticClass());
}

void UBTTask_LimenActivateAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	AbilityInstance.ResolveSelectedKey(*Asset.GetBlackboardAsset());
}

EBTNodeResult::Type UBTTask_LimenActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController* Controller = OwnerComp.GetOwner<AController>();
	if (Controller == nullptr || Controller->GetPawn() == nullptr)
	{
		return EBTNodeResult::Type::Failed;
	}

	UObject* Temp = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityInstance.SelectedKeyName);
	ULimenActiveAbility* AbilityPtr = Cast<ULimenActiveAbility>(Temp);

	if (AbilityPtr == nullptr || !AbilityPtr->CanActivateAbility())
	{
		return EBTNodeResult::Type::Failed;
	}

	if (AbilityPtr->IsActive())
	{
		return EBTNodeResult::Type::Succeeded;
	}

	
	AbilityPtr->ActivateAbility(Controller, Controller->GetPawn());
	return EBTNodeResult::Type::Succeeded;
}
