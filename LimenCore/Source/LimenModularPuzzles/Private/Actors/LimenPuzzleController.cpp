// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LimenPuzzleController.h"

#include "Algo/AllOf.h"
#include "Algo/AnyOf.h"
#include "Algo/Count.h"
#include "DataAssets/LimenPuzzleDefinition.h"
#include "Components/LimenPuzzleElementComponent.h"
#include "Misc/FileHelper.h"
#include "Net/UnrealNetwork.h"
#include "Utils/PuzzleHelpers.h"


ALimenPuzzleController::ALimenPuzzleController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ALimenPuzzleController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bSolved, FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

void ALimenPuzzleController::BeginPlay()
{
	Super::BeginPlay();

	PuzzleDefinitionPtr = TStrongObjectPtr(PuzzleDefinition.Get());
	if (PuzzleDefinitionPtr.IsValid())
	{
		BindElements();
		Recompute();
	}
}

void ALimenPuzzleController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	PuzzleDefinitionPtr.Reset();
	Super::EndPlay(EndPlayReason);
}

void ALimenPuzzleController::AnyPuzzleElementChanged()
{
	Recompute();
}

void ALimenPuzzleController::BindElements()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (auto* PuzzleElement = It->GetComponentByClass<ULimenPuzzleElementComponent>())
		{
			PuzzleElement->OnComponentActivated.AddUniqueDynamic(this, &ThisClass::AnyComponentActivated);
			PuzzleElement->OnComponentDeactivated.AddUniqueDynamic(this, &ThisClass::AnyComponentDeactivated);
			PuzzleElements.Push(TWeakObjectPtr(PuzzleElement));
		}
	}
}

bool ALimenPuzzleController::EvalElementOrNode(const FName& Id)
{
	// 1) element?
	for (auto& E : PuzzleElements)
	{
		if (!E.IsValid() || E->GetElementId() != Id) continue;
		return E->IsActive();
	}

	// 2) node?
	const FPuzzleConditionNode* Node = PuzzleDefinitionPtr->Graph.FindByPredicate(
		[&](const FPuzzleConditionNode& N){ return N.NodeId == Id; });
	if (!Node) return false;

	TArray<bool> inputs;
	inputs.Reserve(Node->InputIDs.Num());
	for (const FName& In : Node->InputIDs) inputs.Add(EvalCached(In));

	switch (Node->Operator)
	{
	case EPuzzleOp::And:     return Algo::AllOf(inputs, [](bool v){ return v; });
	case EPuzzleOp::Or:      return Algo::AnyOf(inputs, [](bool v){ return v; });
	case EPuzzleOp::Xor:     return (Algo::CountIf(inputs, [](bool v){ return v; }) % 2) == 1;
	case EPuzzleOp::AtLeast: return Algo::CountIf(inputs, [](bool v){ return v; }) >= Node->Threshold;
	case EPuzzleOp::TimerGate: return TimerGate(Id, inputs);
	default: return false;
	}
}

bool ALimenPuzzleController::EvalCached(const FName& Id)
{
	if (bool* V = CachedStateValues.Find(Id)) return *V;
	bool V2 = EvalElementOrNode(Id);
	CachedStateValues.Add(Id, V2);
	return V2;
}

void ALimenPuzzleController::Recompute()
{
	CachedStateValues.Reset();
	const bool NewSolved = EvalCached(PuzzleDefinitionPtr->OutputId);
	if (NewSolved != bSolved)
	{
		bSolved = NewSolved;
		OnRep_PuzzleState();
	}
}

void ALimenPuzzleController::ResetTimerGate(FName NodeId)
{
	if (FTimerGateState* S = TimerStates.Find(NodeId))
	{
		S->TrueStartTime = -1.f;
		S->LastOutputTime = -FLT_MAX;
		S->bLatched = false;
	}
}

float ALimenPuzzleController::GetCompletionNormalized() const
{
	if (!PuzzleDefinitionPtr.IsValid()) return 0.f;
	if (!PuzzleElements.IsEmpty()) return 0.f;

	const int32 TotalPuzzleElements = PuzzleElements.Num();
	int32 CompletedPuzzleElements = 0;
	for (auto& StateValue : CachedStateValues)
	{
		if (StateValue.Value) CompletedPuzzleElements++;
	}

	return static_cast<float>(CompletedPuzzleElements) / static_cast<float>(TotalPuzzleElements);
}

void ALimenPuzzleController::OnRep_PuzzleState()
{
	if (bSolved) { PuzzleSolved(); } else { PuzzleUnsolved(); }
}

bool ALimenPuzzleController::TimerGate(const FName& NodeId, const TArray<bool>& Inputs)
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

void ALimenPuzzleController::AnyComponentActivated(UActorComponent* Component, bool bReset)
{
	AnyPuzzleElementChanged();
}

void ALimenPuzzleController::AnyComponentDeactivated(UActorComponent* Component)
{
	AnyPuzzleElementChanged();
}

