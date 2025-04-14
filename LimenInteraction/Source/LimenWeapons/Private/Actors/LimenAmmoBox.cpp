// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenAmmoBox.h"

#include "Components/Interaction/LimenInteractionComponent.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"


ALimenAmmoBox::ALimenAmmoBox()
{
	AmmoCount = 1;
}

void ALimenAmmoBox::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);
	
	ULimenInteractionComponent* InteractionComponent = InPawn->GetComponentByClass<ULimenInteractionComponent>();
	if (InteractionComponent == nullptr)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	for (int i = 1; i < AmmoCount; i++)
	{
		ALimenAmmo* Ammo = GetWorld()->SpawnActor<ALimenAmmo>(AmmoClass, Params);
		InteractionComponent->Interact(InController, InPawn, Ammo);
	}
}
