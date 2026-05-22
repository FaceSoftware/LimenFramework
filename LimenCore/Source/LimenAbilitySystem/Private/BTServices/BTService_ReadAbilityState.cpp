// Copyright FaceSoftware. All Rights Reserved.


#include "BTServices/BTService_ReadAbilityState.h"

#include "Abilities/LimenActiveAbility.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_ReadAbilityState::UBTService_ReadAbilityState()
{
	NodeName = TEXT("Read Ability State");
#if WITH_EDITOR
	bCanTickOnSearchStartBeExposed = true;
#endif
	bNotifyBecomeRelevant = bNotifyTick = bNotifyCeaseRelevant = true;
}

void UBTService_ReadAbilityState::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);
	
	AbilityInstanceKey.ResolveSelectedKey(*Asset.GetBlackboardAsset());
	KeyToStoreValue.ResolveSelectedKey(*Asset.GetBlackboardAsset());
}

void UBTService_ReadAbilityState::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

}

void UBTService_ReadAbilityState::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (AbilityInstanceKey.IsNone()) return;
	if (KeyToStoreValue.IsNone()) return;

	const ULimenActiveAbility* Ability = Cast<ULimenActiveAbility>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AbilityInstanceKey.SelectedKeyName));
	if (!Ability) return;
	OwnerComp.GetBlackboardComponent()->SetValueAsBool(KeyToStoreValue.SelectedKeyName, Ability->IsActive());
}
