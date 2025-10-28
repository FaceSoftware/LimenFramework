// Copyright Face Software. All Rights Reserved.


#include "Interactables/LimenInteractable.h"

#include "Engine/World.h"
#include "GameFramework/PlayerController.h"


ALimenInteractable::ALimenInteractable(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	bWasInteracted = false;
	bIsBeingContinuouslyInteracted = false;
}

void ALimenInteractable::BeginPlay()
{
	Super::BeginPlay();

	for (ILimenInteractableComponent* InteractableAreaComponent : GetInteractableComponents())
	{
		InteractableAreaComponent->GetInteractionDelegate()->AddUObject(this, &ThisClass::OnInteract_Internal);
		InteractableAreaComponent->GetInteractionStoppedDelegate()->AddUObject(this, &ThisClass::OnInteractionStopped_Internal);
	}
}

bool ALimenInteractable::HasBeenInteracted() const
{
	return bWasInteracted;
}

TArray<ILimenInteractableComponent*> ALimenInteractable::GetInteractableComponents() const
{
	TArray<UActorComponent*> InteractableComponents = GetComponentsByInterface(ULimenInteractableComponent::StaticClass());

	TArray<ILimenInteractableComponent*> OutComponents;
	OutComponents.Reserve(InteractableComponents.Num());
	
	for (UActorComponent* InteractableComponent : InteractableComponents)
	{
		checkf(InteractableComponent->Implements<ULimenInteractableComponent>(), TEXT("Interactable components must inherit ILimenInteractableComponent interface"));
		OutComponents.Push(CastChecked<ILimenInteractableComponent>(InteractableComponent));
	}
	
	return OutComponents;
}

bool ALimenInteractable::ShouldSaveData() const
{
	return true;
}

bool ALimenInteractable::ShouldLoadData() const
{
	return true;
}

void ALimenInteractable::PreDataSaved()
{
}

void ALimenInteractable::PostDataLoaded()
{
}

FName ALimenInteractable::GetUniqueDeterministicId() const
{
	return UniqueName;
}

void ALimenInteractable::SetUniqueName(const FName& NewUniqueName)
{
	UniqueName = NewUniqueName;
}

void ALimenInteractable::PostDataSaved()
{
}

void ALimenInteractable::PreDataLoaded()
{
}

bool ALimenInteractable::IsBeingContinuouslyInteracted() const
{
	return bIsBeingContinuouslyInteracted;
}

void ALimenInteractable::Interact(AController* InController, APawn* InPawn)
{
	bWasInteracted = true;
	bIsBeingContinuouslyInteracted = true;
}

void ALimenInteractable::InteractionStopped(AController* InController, APawn* InPawn)
{
	bIsBeingContinuouslyInteracted = false;	
}

void ALimenInteractable::OnInteract_Internal(AController* InController, APawn* InPawn)
{
	Interact(InController, InPawn);
	OnInteract.Broadcast(InController, InPawn);
}

void ALimenInteractable::OnInteractionStopped_Internal(AController* InController, APawn* InPawn)
{
	InteractionStopped(InController, InPawn);
	OnInteractionStopped.Broadcast(InController, InPawn);
}
