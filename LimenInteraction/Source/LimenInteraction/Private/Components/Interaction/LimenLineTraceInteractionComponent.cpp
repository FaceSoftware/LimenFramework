// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenLineTraceInteractionComponent.h"

#include "Items/LimenItemBase.h"
#include "Components/Interactable/LimenInteractableAreaComponent.h"


FCollisionResponseParams ULimenLineTraceInteractionComponent::CollisionResponseParams = FCollisionResponseParams(ECR_Overlap);
FCollisionQueryParams ULimenLineTraceInteractionComponent::CollisionQueryParams = FCollisionQueryParams();

ULimenLineTraceInteractionComponent::ULimenLineTraceInteractionComponent(
	const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	InteractionCollisionChannel = ECC_EngineTraceChannel1;
	InteractionResults.Reserve(32);
	// LineTraceRadius = 1.f;
}

bool ULimenLineTraceInteractionComponent::GetInteractionHitResult(FHitResult& OutHit) const
{
	if (!InteractionHitResult.IsSet())
	{
		return false;
	}

	OutHit = InteractionHitResult.GetValue();
	return true;
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
	if (!GetOwnerLineOfSightStartAndEndVectors(Start, End))
	{
		return;
	}

	GetWorld()->LineTraceMultiByChannel(InteractionResults, Start, End, InteractionCollisionChannel, QueryParams);
	// Line trace goes through some objects for some reason...
	// GetWorld()->SweepMultiByChannel(InteractionResults, Start, End, FQuat::Identity, InteractionCollisionChannel, FCollisionShape::MakeSphere(LineTraceRadius), CollisionQueryParams, CollisionResponseParams);

#if WITH_EDITORONLY_DATA
	
	if (DebugMode())
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, DeltaTime, 0, 0);
	}
	
#endif
	
	CurrentInteractableInterface = nullptr;
	InteractionHitResult.Reset();

	if (!InteractionResults.IsEmpty())
	{
		FHitResult* InteractionHit = nullptr;
		bool bIsHoveringItem = false;
		TScriptInterface<ILimenInteractableComponent> FirstInteractableComponent = nullptr;
		for (FHitResult& Result : InteractionResults)
		{
			// For better compatibility, we only check for the interactable component
			if (!IsInteractableComponent(Result.GetComponent()))
			{
				continue;
			}

			// We give priority to items, and then to other things
			if (Result.GetActor()->GetClass()->IsChildOf(ALimenItemBase::StaticClass()))
			{
				bIsHoveringItem = true;
				InteractionHit = &Result;

				CurrentInteractableInterface.SetObject(Result.GetComponent());
				CurrentInteractableInterface.SetInterface(Cast<ILimenInteractableComponent>(Result.GetComponent()));
				
				break;
			}

			if (FirstInteractableComponent == nullptr)
			{
				FirstInteractableComponent.SetObject(Result.GetComponent());
				FirstInteractableComponent.SetInterface(Cast<ILimenInteractableComponent>(Result.GetComponent()));
				
				InteractionHit = &Result;
			}
		}

		if (!bIsHoveringItem)
		{
			CurrentInteractableInterface = FirstInteractableComponent;
		}

		if (InteractionHit != nullptr)
		{
			InteractionHitResult = *InteractionHit;
		}
	}

#if WITH_EDITORONLY_DATA
	
	if (DebugMode())
	{
		DrawDebugSphere(GetWorld(), InteractionHitResult.IsSet() ? InteractionHitResult.GetValue().ImpactPoint : End,
						16.f, 4, FColor::Blue, false, DeltaTime, 0.f, 0.f);
	}
	
#endif WITH_EDITORONLY_DATA
	
}
