// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/LimenSnapAnchorComponent.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
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
#if WITH_EDITORONLY_DATA
	TargetAnchorId = NAME_None;
	bAttach = true;
#endif

	Mobility = EComponentMobility::Movable;
}

void ULimenSnapAnchorComponent::AnchorTo(ULimenAnchorPointComponent* InTarget, const bool bAttachToAnchor)
{
	if (!InTarget || InTarget->GetOwner() == GetOwner()) return;

	RemoveCurrentAnchor();
	InTarget->Anchor(this, bAttachToAnchor);
	OnAnchored.Broadcast();
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

#if WITH_EDITORONLY_DATA

void ULimenSnapAnchorComponent::DEBUG_AnchorToTarget()
{
	if (AnchorTargetActor.IsValid()) AnchorToActor(AnchorTargetActor.Get(), TargetAnchorId, bAttach);
}

void ULimenSnapAnchorComponent::DEBUG_RemoveAnchor()
{
	RemoveCurrentAnchor(true);
}

ULimenAnchorPointComponent* ULimenSnapAnchorComponent::GetCurrentAnchor() const
{
	return CurrentAnchor.Get();
}

#endif


/// ULimenAnchorPointComponent


ULimenAnchorPointComponent::ULimenAnchorPointComponent()
{
	AnchorId = NAME_None;
	Mobility = EComponentMobility::Movable;
}

void ULimenAnchorPointComponent::Anchor(ULimenSnapAnchorComponent* InRequestor, const bool bAttachToAnchor)
{
	if (!InRequestor || !InRequestor->GetOwner()) return;

	const FTransform TargetTransform = GetComponentTransform();
	const FTransform RequestorAnchorTransform = InRequestor->GetComponentTransform();

	const FVector DeltaLoc = TargetTransform.GetLocation() - RequestorAnchorTransform.GetLocation();
	const FRotator DeltaRot = TargetTransform.Rotator() - RequestorAnchorTransform.Rotator();

	if (DeltaLoc.SizeSquared() < KINDA_SMALL_NUMBER && DeltaRot.IsNearlyZero())
		return;

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


const FName& ULimenAnchorPointComponent::GetAnchorId() const
{
	return AnchorId;
}

const TArray<ULimenSnapAnchorComponent*>& ULimenAnchorPointComponent::GetAnchoredComponents() const
{
	return AnchoredComponents;
}
