// Copyright Face Software. All Rights Reserved.


#include "SavesHandlers/LimenItemSavesHandler.h"

#include "..\..\..\LimenPlayers\Public\Characters\LimenPlayerCharacter.h"
#include "Components/LimenInventoryComponent.h"
#include "Actors/LimenTool.h"
#include "Actors/LimenWeapon.h"
#include "Components/LimenPhysicalItemHoldComponent.h"
#include "SaveGames/LimenSaveData.h"

void ULimenItemSavesHandler::SaveDataFrom(UWorld* World)
{
	const ULimenInventoryComponent* PlayerInventory = World->GetFirstPlayerController()->GetPawn()->GetComponentByClass<ULimenInventoryComponent>();
	const TArray<ALimenItemBase*> Items = PlayerInventory->SaveInventory();
	
	for (ALimenItemBase* Item : Items)
	{
		ItemsSaveData.Push(FActorSaveData(Item));
	}

	if (const ALimenPlayerCharacter* Character = Cast<ALimenPlayerCharacter>(World->GetFirstPlayerController()->GetPawn()); Character != nullptr)
	{
		const ALimenPhysicalItem* Tool = Character->GetToolHoldComponent()->GetPhysicalItem();
		HoldingToolClass = Tool == nullptr ? nullptr : Tool->GetClass();
		const ALimenPhysicalItem* Weapon = Character->GetWeaponHoldComponent()->GetPhysicalItem();
		HoldingWeaponClass = Weapon == nullptr ? nullptr : Weapon->GetClass();
	}
}

void ULimenItemSavesHandler::LoadDataTo(UWorld* World)
{
	ULimenInventoryComponent* PlayerInventory = World->GetFirstPlayerController()->GetPawn()->GetComponentByClass<ULimenInventoryComponent>();

	TArray<TSubclassOf<ALimenItemBase>> SavedPlayerItems;
	for (const FActorSaveData& ItemSave : ItemsSaveData)
	{
		SavedPlayerItems.Push(ItemSave.GetActorClass());	
	}

	TArray<ALimenItemBase*> LoadedItems = PlayerInventory->LoadInventory(SavedPlayerItems);
	for (ALimenItemBase* Item : LoadedItems)
	{
		for (const FActorSaveData& ItemData : ItemsSaveData)
		{
			if (Item->GetClass() == ItemData.GetActorClass())
			{
				ItemData.LoadData(Item);
			}
		}
	}

	ALimenPlayerCharacter* Character = Cast<ALimenPlayerCharacter>(World->GetFirstPlayerController()->GetPawn());
	if (!HoldingToolClass.IsNull())
	{
		ALimenTool* Item = PlayerInventory->PeekItemInstance<ALimenTool>(HoldingToolClass.LoadSynchronous());
		Character->HoldTool(Cast<ALimenTool>(Item));
	}

	if (!HoldingWeaponClass.IsNull())
	{
		ALimenWeapon* Weapon = PlayerInventory->PeekItemInstance<ALimenWeapon>(HoldingWeaponClass.LoadSynchronous());
		Character->HoldWeapon(Weapon);
	}
}
