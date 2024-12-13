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

	SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
}

void ULimenInteractableAreaComponent::Deactivate()
{
	Super::Deactivate();

	SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
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
