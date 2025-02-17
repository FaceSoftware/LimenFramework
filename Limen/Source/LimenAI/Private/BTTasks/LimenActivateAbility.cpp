// Copyright © 2024 FaceSoftware. All rights reserved.


#include "BTTasks/LimenActivateAbility.h"

#include "Abilities/LimenAbilityBase.h"
#include "Abilities/LimenActiveAbility.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/PlayerController.h"
#include "LimenAbilitySystem/Public/Components/LimenAbilityComponent.h"


ULimenActivateAbility::ULimenActivateAbility()
{
	NodeName = TEXT("Activate Limen Ability");
	AbilityInstance.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ULimenActivateAbility, AbilityInstance), ULimenActiveAbility::StaticClass());
}

EBTNodeResult::Type ULimenActivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
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
		return EBTNodeResult::Type::Aborted;
	}

	
	AbilityPtr->ActivateAbility(Controller, Controller->GetPawn());
	return EBTNodeResult::Type::Succeeded;
}
