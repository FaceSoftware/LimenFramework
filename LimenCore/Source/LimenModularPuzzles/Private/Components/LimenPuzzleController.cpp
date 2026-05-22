// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenPuzzleController.h"

#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
#include "Algo/Count.h"
#include "DataAssets/LimenPuzzleDefinition.h"
#include "Components/LimenPuzzleElementComponent.h"
#include "Misc/FileHelper.h"
#include "Net/UnrealNetwork.h"
#include "Utils/PuzzleHelpers.h"
#include "EngineUtils.h"


ULimenPuzzleController::ULimenPuzzleController()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void ULimenPuzzleController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bSolved, FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

void ULimenPuzzleController::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		PuzzleDefinitionPtr = TStrongObjectPtr(PuzzleDefinition.Get());
		if (PuzzleDefinitionPtr.IsValid())
		{
			BindElements();
			Recompute();
		}
	}
}

void ULimenPuzzleController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PuzzleDefinitionPtr.Reset();
	Super::EndPlay(EndPlayReason);
}

void ULimenPuzzleController::AnyPuzzleElementChanged()
{
	Recompute();
	OnPuzzleElementUpdated.Broadcast(this);
}

void ULimenPuzzleController::BindElements()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		auto* PuzzleElement = It->GetComponentByClass<ULimenPuzzleElementComponent>();
		if (!PuzzleElement) continue;

		if (!PuzzleDefinitionPtr->PuzzleElementIds.Contains(PuzzleElement->GetElementId())) continue;

		PuzzleElement->OnComponentActivated.AddUniqueDynamic(this, &ThisClass::AnyComponentActivated);
		PuzzleElement->OnComponentDeactivated.AddUniqueDynamic(this, &ThisClass::AnyComponentDeactivated);
		PuzzleElements.Push(TWeakObjectPtr(PuzzleElement));
	}
}

bool ULimenPuzzleController::EvalElementOrNode(const FName& Id)
{
	// 1) element?
	for (auto& Element : PuzzleElements)
	{
		if (!Element.IsValid() || Element->GetElementId() != Id) continue;
		return Element->IsActive();
	}

	// 2) node?
	const FPuzzleConditionNode* Node = PuzzleDefinitionPtr->Graph.FindByPredicate(
		[&](const FPuzzleConditionNode& N)
		{
			return N.NodeId == Id;
		});

	if (!Node) return false;

	TArray<bool> Inputs;
	Inputs.Reserve(Node->InputIDs.Num());
	for (const FName& In : Node->InputIDs)
	{
		Inputs.Add(EvalCached(In));
	}

	switch (Node->Operator)
	{
	case EPuzzleOp::And:
		return Algo::AllOf(Inputs, [](bool v){ return v; });

	case EPuzzleOp::Or:
	    return Algo::AnyOf(Inputs, [](bool v){ return v; });

	case EPuzzleOp::Xor:
	    return (Algo::CountIf(Inputs, [](bool v){ return v; }) % 2) == 1;

	case EPuzzleOp::AtLeast:
		return Algo::CountIf(Inputs, [](bool v){ return v; }) >= Node->Threshold;

	case EPuzzleOp::TimerGate:
		return TimerGate(Id, Inputs);

	default:
		return false;
	}
}

bool ULimenPuzzleController::EvalCached(const FName& Id)
{
	if (const bool* V = CachedStateValues.Find(Id)) return *V;

	const bool V2 = EvalElementOrNode(Id);
	CachedStateValues.Add(Id, V2);
	return V2;
}

void ULimenPuzzleController::Recompute()
{
	CachedStateValues.Reset();
	if (const bool NewSolved = EvalCached(PuzzleDefinitionPtr->OutputId); NewSolved != bSolved)
	{
		bSolved = NewSolved;
		OnRep_PuzzleState();
	}
}

void ULimenPuzzleController::ResetTimerGate(FName NodeId)
{
	if (FTimerGateState* S = TimerStates.Find(NodeId))
	{
		S->TrueStartTime = -1.f;
		S->LastOutputTime = -FLT_MAX;
		S->bLatched = false;
	}
}

float ULimenPuzzleController::GetCompletionNormalized() const
{
	if (!PuzzleDefinitionPtr.IsValid()) return 0.f;
	if (PuzzleElements.IsEmpty()) return 0.f;

	const int32 TotalPuzzleElements = PuzzleElements.Num();
	int32 CompletedPuzzleElements = 0;
	for (auto& StateValue : CachedStateValues)
	{
		if (StateValue.Value) CompletedPuzzleElements++;
	}

	return static_cast<float>(CompletedPuzzleElements) / static_cast<float>(TotalPuzzleElements);
}

void ULimenPuzzleController::OnRep_PuzzleState()
{
	OnPuzzleStateChanged.Broadcast(bSolved);
}

bool ULimenPuzzleController::TimerGate(const FName& NodeId, const TArray<bool>& Inputs)
{
	const FPuzzleConditionNode* Node = PuzzleDefinitionPtr->Graph.FindByPredicate(
		[&](const FPuzzleConditionNode& N){ return N.NodeId == NodeId; });
	if (!Node) return false;

	const bool bAllTrue = Algo::AllOf(Inputs, [](bool v){ return v; });
	FTimerGateState& S = TimerStates.FindOrAdd(NodeId);
	const float Now = GetWorld()->GetTimeSeconds();

	// LATCH MODE: once fired, stays true until ResetTimerGate(NodeId) is called.
	if (Node->bLatch)
	{
		if (S.bLatched) return true;

		if (bAllTrue)
		{
			if (S.TrueStartTime < 0.f) S.TrueStartTime = Now;
			if (Now - S.TrueStartTime >= Node->Duration)
			{
				S.bLatched = true;
				return true;
			}
			return false;
		}
		// Lost condition, reset the continuous-true timer
		S.TrueStartTime = -1.f;
		return false;
	}

	// HOLD MODE: true only while condition has been true for Duration; false otherwise.
	if (bAllTrue)
	{
		if (S.TrueStartTime < 0.f) S.TrueStartTime = Now;

		if (Now - S.TrueStartTime >= Node->Duration)
		{
			// Optional rate limit
			if (Node->Cooldown > 0.f && (Now - S.LastOutputTime) < Node->Cooldown)
				return false;

			S.LastOutputTime = Now;
			return true;
		}
		return false;
	}

	// Condition went false: clear timer and output false.
	S.TrueStartTime = -1.f;
	return false;
}

void ULimenPuzzleController::AnyComponentActivated(UActorComponent* Component, bool bReset)
{
	AnyPuzzleElementChanged();
}

void ULimenPuzzleController::AnyComponentDeactivated(UActorComponent* Component)
{
	AnyPuzzleElementChanged();
}

