// Copyright Face Software. All Rights Reserved.


#include "Components/Interactable/LimenInteractableAreaComponent.h"


ULimenInteractableAreaComponent::ULimenInteractableAreaComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetGenerateOverlapEvents(true);
}

void ULimenInteractableAreaComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	SetCollisionProfileName(TEXT("OverlapAllDynamic"));
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
