// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenAmmoBox.h"

#include "Components/Interaction/LimenInteractionComponent.h"


ALimenAmmoBox::ALimenAmmoBox()
{
	AmmoCount = 1;
}

void ALimenAmmoBox::BeginPlay()
{
	Super::BeginPlay();

	AmmoInstances.Reserve(AmmoCount);
	for (int i = 0; i < AmmoCount; ++i)
	{
		ALimenAmmo* AmmoInstance = GetWorld()->SpawnActor<ALimenAmmo>(AmmoClass);
		AmmoInstance->RemoveFromGameplay();
		AmmoInstances.Push(AmmoInstance);
	}
	
	for (UActorComponent* Component : GetComponentsByInterface(ULimenInteractableComponent::StaticClass()))
	{
		ILimenInteractableComponent* InteractableComponent = Cast<ILimenInteractableComponent>(Component);
		check(InteractableComponent != nullptr);
		InteractableComponent->GetInteractionDelegate()->AddUObject(this, &ALimenAmmoBox::Interact);
	}
}

void ALimenAmmoBox::Interact(AController* InController, APawn* InPawn)
{
	RemoveFromGameplay();
	
	ULimenInteractionComponent* InteractionComponent = InPawn->GetComponentByClass<ULimenInteractionComponent>();
	check(InteractionComponent != nullptr);

	for (ALimenAmmo* Instance : AmmoInstances)
	{
		InteractionComponent->Interact(InController, InPawn, Instance);
	}
}
