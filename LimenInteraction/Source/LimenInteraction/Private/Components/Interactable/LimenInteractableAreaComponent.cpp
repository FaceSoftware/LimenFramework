// Copyright Face Software. All Rights Reserved.


#include "Components/Interactable/LimenInteractableAreaComponent.h"


ULimenInteractableAreaComponent::ULimenInteractableAreaComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetGenerateOverlapEvents(true);
}

void ULimenInteractableAreaComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ULimenInteractableAreaComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	if (!CachedCollisionEnabledType.IsSet() || !CachedCollisionResponse.IsSet())
	{
		CachedCollisionResponse = GetCollisionResponseToChannels();
		CachedCollisionEnabledType = GetCollisionEnabled();
	}

	SetCollisionEnabled(CachedCollisionEnabledType.GetValue());
	SetCollisionResponseToChannels(CachedCollisionResponse.GetValue());
}

void ULimenInteractableAreaComponent::Deactivate()
{
	Super::Deactivate();

	CachedCollisionResponse = GetCollisionResponseToChannels();
	CachedCollisionEnabledType = GetCollisionEnabled();
	
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void ULimenInteractableAreaComponent::NotifyHover(AController* Controller, APawn* Pawn)
{
	BP_OnHoverStart.Broadcast(Controller, Pawn);
}

void ULimenInteractableAreaComponent::NotifyUnHover(AController* Controller, APawn* Pawn)
{
	BP_OnHoverEnd.Broadcast(Controller, Pawn);
}

FInteractableComponentDelegate* ULimenInteractableAreaComponent::GetInteractionDelegate()
{
	return &OnInteract;
}

FInteractableComponentDelegate* ULimenInteractableAreaComponent::GetInteractionStoppedDelegate()
{
	return &OnInteractionStopped;
}

FBPInteractableComponentDelegate* ULimenInteractableAreaComponent::GetBlueprintInteractionDelegate()
{
	return &BP_OnInteract;
}

FBPInteractableComponentDelegate* ULimenInteractableAreaComponent::GetBlueprintInteractionStoppedDelegate()
{
	return &BP_OnInteractionStopped;
}

UPrimitiveComponent* ULimenInteractableAreaComponent::GetPrimitiveComponent()
{
	return this;
}

void ULimenInteractableAreaComponent::Interact(AController* InController, APawn* InPawn)
{
	OnInteract.Broadcast(InController, InPawn);
	BP_OnInteract.Broadcast(InController, InPawn);
}

void ULimenInteractableAreaComponent::StopInteraction(AController* InController, APawn* InPawn)
{
	OnInteractionStopped.Broadcast(InController, InPawn);
	BP_OnInteractionStopped.Broadcast(InController, InPawn);
}
