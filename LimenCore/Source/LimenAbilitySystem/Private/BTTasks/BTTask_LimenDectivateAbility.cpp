// Copyright © 2024 FaceSoftware. All rights reserved.


#include "BTTasks/BTTask_LimenDeactivateAbility.h"

#include "Abilities/LimenActiveAbility.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/PlayerController.h"


UBTTask_LimenDeactivateAbility::UBTTask_LimenDeactivateAbility()
{
	NodeName = TEXT("Deactivate Limen Ability");
	AbilityInstance.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(UBTTask_LimenDeactivateAbility, AbilityInstance), ULimenActiveAbility::StaticClass());
}

void UBTTask_LimenDeactivateAbility::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	AbilityInstance.ResolveSelectedKey(*Asset.GetBlackboardAsset());
}

EBTNodeResult::Type UBTTask_LimenDeactivateAbility::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AController* Controller = OwnerComp.GetOwner<AController>();
	if (Controller == nullptr || Controller->GetPawn() == nullptr) return EBTNodeResult::Type::Failed;
	if (AbilityInstance.IsNone()) return EBTNodeResult::Type::Failed;

	UObject* Temp = OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityInstance.SelectedKeyName);
	ULimenActiveAbility* AbilityPtr = Cast<ULimenActiveAbility>(Temp);

	if (!AbilityPtr) return EBTNodeResult::Type::Failed;
	if (!AbilityPtr->IsActive()) return EBTNodeResult::Type::Succeeded;
	
	AbilityPtr->CancelAbility(Controller, Controller->GetPawn());
	return EBTNodeResult::Type::Succeeded;
}
