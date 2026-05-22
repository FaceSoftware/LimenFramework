// Copyright © FaceSoftware. All rights reserved.


#include "BTService/BTService_GetActorLocation.h"

#include "BehaviorTree/BlackboardComponent.h"


UBTService_GetActorLocation::UBTService_GetActorLocation()
{
	Target.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, Target), AActor::StaticClass());
	OutLocation.AddVectorFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, OutLocation));
}

void UBTService_GetActorLocation::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);

}

void UBTService_GetActorLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const UBlackboardData& Data = *OwnerComp.GetBlackboardComponent()->GetBlackboardAsset();
	Target.ResolveSelectedKey(Data);
	OutLocation.ResolveSelectedKey(Data);

	if (Target.IsNone() || !Target.IsSet() || !OutLocation.IsSet()) return;

	UObject* TargetObject = OwnerComp.GetBlackboardComponent()->GetValueAsObject(Target.SelectedKeyName);
	const AActor* TargetActor = CastChecked<AActor>(TargetObject);
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(OutLocation.SelectedKeyName,
														 TargetActor->GetActorLocation());
}
