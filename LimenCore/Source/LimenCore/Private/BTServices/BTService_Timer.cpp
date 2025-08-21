// Copyright FaceSoftware. All Rights Reserved.


#include "BTServices/BTService_Timer.h"


UBTService_Timer::UBTService_Timer()
{
	NodeName = TEXT("Timer Service");
	Interval = 0.1f;
	bNotifyBecomeRelevant = bNotifyTick = bNotifyCeaseRelevant = true;

	Duration = 5.f;
	bLoop = false;
}

void UBTService_Timer::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	if (const UBlackboardData* BB = GetBlackboardAsset()) DoneKey.ResolveSelectedKey(*BB);
}

void UBTService_Timer::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	auto* Mem = reinterpret_cast<FTimerMem*>(NodeMemory);
	Mem->Deadline = OwnerComp.GetWorld()->GetTimeSeconds() + Duration.GetValue(OwnerComp);
	Mem->bFired = false;

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent(); BB && DoneKey.SelectedKeyType)
	{
		BB->ClearValue(DoneKey.SelectedKeyName);
	}
}

void UBTService_Timer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	auto* Mem = reinterpret_cast<FTimerMem*>(NodeMemory);
	if (Mem->Deadline < 0.f) return;

	const float Now = OwnerComp.GetWorld()->GetTimeSeconds();
	if (Now >= Mem->Deadline)
	{
		if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent(); BB && DoneKey.SelectedKeyType)
		{
			BB->SetValueAsBool(DoneKey.SelectedKeyName, true);
		}

		if (bLoop)
		{
			Mem->Deadline = Now + Duration.GetValue(OwnerComp);
		}
		else
		{
			Mem->Deadline = -1.f;
			Mem->bFired = true;
		}
	}
}

void UBTService_Timer::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	auto* Mem = reinterpret_cast<FTimerMem*>(NodeMemory);
	Mem->Deadline = -1.f;
	Mem->bFired = false;

	if (UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent(); BB && DoneKey.SelectedKeyType)
	{
		BB->SetValueAsBool(DoneKey.SelectedKeyName, false);
	}
}
