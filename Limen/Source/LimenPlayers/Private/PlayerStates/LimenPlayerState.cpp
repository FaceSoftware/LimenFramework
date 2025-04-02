// Copyright Face Software. All Rights Reserved.


#include "PlayerStates/LimenPlayerState.h"

#include "Actors/LimenTool.h"
#include "Actors/LimenWeapon.h"
#include "Characters/LimenPlayerCharacter.h"
#include "Components/LimenInventoryComponent.h"
#include "Components/LimenObjectiveComponent.h"
#include "Components/LimenPhysicalItemHoldComponent.h"
#include "GameFramework/PlayerController.h"
#include "Managers/LimenObjectivesManager.h"


ALimenPlayerState::ALimenPlayerState(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	SetReplicates(true);
}

void ALimenPlayerState::BeginPlay()
{
	Super::BeginPlay();

	if (GetPawn())
	{
		OwnerPlayerController = Cast<APlayerController>(GetPawn()->GetController());
	}

	if (const ALimenPlayerCharacter* LimenPawn = Cast<ALimenPlayerCharacter>(GetPawn());
		LimenPawn != nullptr)
	{
		LimenPawn->GetInventoryComponent()->OnInventoryUpdated.AddUniqueDynamic(this, &ThisClass::InventoryUpdated);
		InventoryUpdated(LimenPawn->GetInventoryComponent());
		
		LimenPawn->GetToolHoldComponent()->OnItemChanged.AddUniqueDynamic(this, &ThisClass::CurrentHoldingToolChanged);
		LimenPawn->GetWeaponHoldComponent()->OnItemChanged.AddUniqueDynamic(this, &ThisClass::CurrentHoldingWeaponChanged);
	}

	if (ALimenObjectivesManager* ObjectivesManager = ALimenGameplayManager::GetGameplayManager<ALimenObjectivesManager>(GetWorld());
		ObjectivesManager != nullptr)
	{
		if (ObjectivesManager->HasInitialized())
		{
			ObjectivesManagerInitialized(ObjectivesManager);
		}
		else
		{
			ObjectivesManager->OnObjectInitialized.AddUObject(this, &ThisClass::ObjectivesManagerInitialized);
		} 
	}
}

void ALimenPlayerState::CurrentHoldingToolChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New)
{	
	if (Old == New)
	{
		return;
	}

	if (New == nullptr)
	{
		CurrentHoldingTool = nullptr;
		return;
	}
	
	CurrentHoldingTool = static_cast<ALimenTool*>(New);
	check(IsValid(CurrentHoldingTool))
}

void ALimenPlayerState::CurrentHoldingWeaponChanged(ALimenPhysicalItem* Old, ALimenPhysicalItem* New)
{
	if (Old == New)
	{
		return;
	}

	if (New == nullptr)
	{
		CurrentHoldingWeapon = nullptr;
		return;
	}
	
	CurrentHoldingWeapon = static_cast<ALimenWeapon*>(New);
	check(CurrentHoldingWeapon != nullptr);
}

void ALimenPlayerState::InventoryUpdated(ULimenInventoryComponent* Inventory)
{
	InventoryItems.Empty(Inventory->PeekInventory().Num());
	for (TTuple<TSubclassOf<ALimenItemBase>, int>& Item : Inventory->PeekInventory())
	{
		InventoryItems.Push(Item.Key);
	}
}

void ALimenPlayerState::ObjectiveAdded(ALimenObjective* Objective, const FObjectiveData& Data)
{
	PlayerObjectives.Add(Objective->GetClass(), Data);
}

void ALimenPlayerState::ObjectiveUpdated(ALimenObjective* Objective, const FObjectiveData& Data)
{
	if (PlayerObjectives.Contains(Objective->GetClass()))
	{
		PlayerObjectives[Objective->GetClass()] = Data;
	}
}

void ALimenPlayerState::ObjectivesManagerInitialized(UObject* ObjectivesManagerObject)
{
	const ALimenObjectivesManager* ObjectivesManager = CastChecked<ALimenObjectivesManager>(ObjectivesManagerObject);
	for (auto* Objective : ObjectivesManager->GetObjectivesInstances())
	{
		Objective->OnObjectiveAdded.AddUniqueDynamic(this, &ThisClass::ALimenPlayerState::ObjectiveAdded);
		Objective->OnObjectiveUpdated.AddUniqueDynamic(this, &ThisClass::ALimenPlayerState::ObjectiveUpdated);
	}

	if (GetPawn() == nullptr)
	{
		return;
	}
	
	const APlayerController* PlayerController = Cast<APlayerController>(GetPawn()->GetController());
	if (PlayerController == nullptr)
	{
		return;
	}
	
	for (const ALimenObjective* Objective : ObjectivesManager->GetObjectivesForPlayer(PlayerController, GetPawn()))
	{
		PlayerObjectives.Add(Objective->GetClass(), Objective->GetObjectiveData());
	}
}
