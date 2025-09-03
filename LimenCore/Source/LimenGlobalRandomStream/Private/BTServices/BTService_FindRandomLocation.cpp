// Copyright FaceSoftware. All Rights Reserved.


#include "BTServices/BTService_FindRandomLocation.h"

#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"


UBTService_FindRandomLocation::UBTService_FindRandomLocation()
{
	MaxAttempts = 16;
	MaxRadius = 10000;
	MinRadius = 5000;
	AcceptanceRadius = 100;

	bCanTickOnSearchStartBeExposed = true;
	SearchOrigin.AddObjectFilter(this, GET_MEMBER_NAME_CHECKED(ThisClass, SearchOrigin), AActor::StaticClass());
}

void UBTService_FindRandomLocation::OnSearchStart(FBehaviorTreeSearchData& SearchData)
{
	Super::OnSearchStart(SearchData);
}

void UBTService_FindRandomLocation::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	UNavigationSystemV1* NavSys = FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());
	if (NavSys == nullptr) return;

	const ANavigationData* NavData = NavSys->GetDefaultNavDataInstance(FNavigationSystem::DontCreate);
	if (NavData == nullptr) return;

	SearchOrigin.ResolveSelectedKey(*OwnerComp.GetBlackboardComponent()->GetBlackboardAsset());
	if (SearchOrigin.IsNone()) return;

	const FName KeyName = SearchOrigin.SelectedKeyName;
	const AActor* KeyAsActor = CastChecked<AActor>(OwnerComp.GetBlackboardComponent()->GetValueAsObject(KeyName));
	const FVector SearchOriginVector = KeyAsActor->GetActorLocation();

	FNavLocation Location;
	bool bLocationFound = false;
	for (int i = 0; i < MaxAttempts; ++i)
	{
		NavData->GetRandomReachablePointInRadius(SearchOriginVector, MaxRadius, Location);
		if (FVector::DistSquared(SearchOriginVector, Location.Location) >= FMath::Square(MinRadius))
		{
			bLocationFound = true;
			break;
		}
	}

	if (!bLocationFound) return;
	OwnerComp.GetBlackboardComponent()->SetValueAsVector(OutLocation.SelectedKeyName, Location.Location);
}
