// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenLineTraceInteractionComponent.h"

#include "DrawDebugHelpers.h"
#include "Items/LimenItemBase.h"
#include "Components/Interactable/LimenInteractableComponent.h"
#include "Engine/World.h"


FCollisionResponseParams ULimenLineTraceInteractionComponent::CollisionResponseParams = FCollisionResponseParams(ECR_Overlap);
FCollisionQueryParams ULimenLineTraceInteractionComponent::CollisionQueryParams = FCollisionQueryParams();

ULimenLineTraceInteractionComponent::ULimenLineTraceInteractionComponent(
	const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InteractionCollisionChannel = ECC_EngineTraceChannel1;
	InteractionResults.Reserve(32);
	// LineTraceRadius = 1.f;
}

ECollisionChannel ULimenLineTraceInteractionComponent::GetTraceChannel() const
{
	return InteractionCollisionChannel;
}

void ULimenLineTraceInteractionComponent::SetupInteraction()
{
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.bTraceComplex = false;
	QueryParams.bIgnoreBlocks = false;
	QueryParams.bIgnoreTouches = false;
}

void ULimenLineTraceInteractionComponent::UpdateInteraction(const float DeltaTime)
{	
	FVector Start, End;
	if (UNLIKELY(!GetOwnerViewPoint(Start, End))) { return; }

	GetWorld()->LineTraceMultiByChannel(InteractionResults, Start, End, InteractionCollisionChannel, QueryParams);
	const FHitResult* InteractionHit = nullptr;
	if (!InteractionResults.IsEmpty())
	{
		for (FHitResult& Result : InteractionResults)
		{
			auto* InteractableComponent = Cast<ULimenInteractableComponent>(Result.GetComponent());
			if (LIKELY(!InteractableComponent)) { continue; }
			InteractionHit = &Result;
			SetCurrentInteractable(InteractableComponent);
			break;
		}
	}

#if WITH_EDITORONLY_DATA
	if (DebugMode())
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, DeltaTime, 0, 0);
	}
	
	if (DebugMode())
	{
		DrawDebugSphere(GetWorld(), InteractionHit != nullptr ? FVector(InteractionHit->ImpactPoint) : End,
						16.f, 4, FColor::Blue, false, DeltaTime, 0.f, 0.f);
	}
#endif
}
