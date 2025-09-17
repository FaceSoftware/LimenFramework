// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenSnapAnchorComponent.h"

#include "Components/ArrowComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "LogMacros/LimenLogMacros.h"


ULimenSnapAnchorComponent* ULimenSnapAnchorComponent::GetAnchorByTargetId(const AActor* InActor, const FName& TargetId)
{
	if (InActor)
	{
		TArray<ULimenSnapAnchorComponent*> Components;
		InActor->GetComponents<ULimenSnapAnchorComponent>(Components);

		for (ULimenSnapAnchorComponent*& Component : Components)
		{
			if (Component->GetTargetAnchorId() == TargetId) return Component;
		}
	}

	return nullptr;
}

ULimenSnapAnchorComponent::ULimenSnapAnchorComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	TargetAnchorId = NAME_None;
	bAttach = true;

	Mobility = EComponentMobility::Movable;
}

void ULimenSnapAnchorComponent::AnchorTo(ULimenAnchorPointComponent* InTarget, const bool bAttachToAnchor)
{
	if (!InTarget || InTarget->GetOwner() == GetOwner()) return;

	RemoveCurrentAnchor();
	if (InTarget->Anchor(this, bAttachToAnchor))
	{
		CurrentAnchor = InTarget;
		OnAnchored.Broadcast();
	}
}

void ULimenSnapAnchorComponent::AnchorToActor(AActor* InTarget, FName AnchorId, const bool bAttachToAnchor)
{
	if (!InTarget || InTarget == GetOwner()) return;

	TArray<ULimenAnchorPointComponent*> Components;
	InTarget->GetComponents<ULimenAnchorPointComponent>(Components);

	for (ULimenAnchorPointComponent* const& Component : Components)
	{
		if (Component && Component->GetAnchorId() == AnchorId) AnchorTo(Component, bAttachToAnchor);
	}
}

void ULimenSnapAnchorComponent::RemoveCurrentAnchor(const bool bDetachFromAnchor)
{
	if (!CurrentAnchor.IsValid()) return;

	CurrentAnchor->RemoveAnchor(this, bDetachFromAnchor);
}

const FName& ULimenSnapAnchorComponent::GetTargetAnchorId() const
{
	return TargetAnchorId;
}

ULimenAnchorPointComponent* ULimenSnapAnchorComponent::GetCurrentAnchor() const
{
	return CurrentAnchor.Get();
}

void ULimenSnapAnchorComponent::DEBUG_AnchorToTarget()
{
	if (AnchorTargetActor.IsValid()) AnchorToActor(AnchorTargetActor.Get(), TargetAnchorId, bAttach);
}

void ULimenSnapAnchorComponent::DEBUG_RemoveAnchor()
{
	RemoveCurrentAnchor(true);
}

///
/// ULimenAnchorPointComponent
///

ULimenAnchorPointComponent::ULimenAnchorPointComponent()
{
	AnchorId = NAME_None;
	Mobility = EComponentMobility::Movable;
}

bool ULimenAnchorPointComponent::Anchor(ULimenSnapAnchorComponent* InRequestor, const bool bAttachToAnchor)
{
	if (!InRequestor || !InRequestor->GetOwner()) return false;

	const FTransform TargetTransform = GetComponentTransform();
	const FTransform RequestorAnchorTransform = InRequestor->GetComponentTransform();

	const FTransform SnapOffset = RequestorAnchorTransform.GetRelativeTransform(InRequestor->GetOwner()->GetActorTransform());
	const FTransform FinalTransform = UKismetMathLibrary::ComposeTransforms(SnapOffset.Inverse(), TargetTransform);

	InRequestor->GetOwner()->SetActorTransform(FinalTransform);

	if (bAttachToAnchor)
	{
		InRequestor->GetOwner()->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
	}

	AnchoredComponents.AddUnique(InRequestor);
	OnNewAnchoredActor.Broadcast(InRequestor);

	LIMEN_LOG(LogLimen, Log, this, TEXT("Snapped %s to anchor %s."), *InRequestor->GetOwner()->GetName(), *GetName());
	return true;
}

void ULimenAnchorPointComponent::RemoveAnchor(ULimenSnapAnchorComponent* InRequestor, const bool bDetachFromAnchor)
{
	if (bDetachFromAnchor && InRequestor && InRequestor->GetOwner())
	{
		InRequestor->GetOwner()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	if (const int32 Index = AnchoredComponents.IndexOfByKey(InRequestor); Index != INDEX_NONE)
	{
		AnchoredComponents.RemoveAt(Index);
		OnActorAnchorRemoved.Broadcast(InRequestor);
	}
}

bool ULimenAnchorPointComponent::HasAnyAnchoredComponents() const
{
	return !AnchoredComponents.IsEmpty();
}


const FName& ULimenAnchorPointComponent::GetAnchorId() const
{
	return AnchorId;
}

const TArray<ULimenSnapAnchorComponent*>& ULimenAnchorPointComponent::GetAnchoredComponents() const
{
	return AnchoredComponents;
}
