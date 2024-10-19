// Copyright Face Software. All Rights Reserved.


#include "Items/LimenItemBase.h"

#include "Components/LimenInventoryComponent.h"
#include "ItemActions/LimenItemAction.h"


UTexture2D* ALimenItemBase::GetItemImage(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	return ItemClass->GetDefaultObject<ALimenItemBase>()->GetItemImage();
}

FText ALimenItemBase::GetDisplayName(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	return ItemClass->GetDefaultObject<ALimenItemBase>()->GetDisplayName();
}

FText ALimenItemBase::GetDescription(const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	return ItemClass->GetDefaultObject<ALimenItemBase>()->GetDescription();
}

TArray<ULimenItemAction*> ALimenItemBase::GetItemActions(ULimenInventoryComponent* InventoryComponent, const TSubclassOf<ALimenItemBase>& ItemClass)
{
	check(ItemClass != nullptr);
	ALimenItemBase* Item = InventoryComponent->PeekItemInstance<ALimenItemBase>(ItemClass);
	check(Item != nullptr);	
	return Item->GetItemActions();
}

ALimenItemBase::ALimenItemBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bHasBeenLoaded = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ALimenItemBase::BeginPlay()
{
	Super::BeginPlay();

	for (const TSubclassOf<ULimenItemAction>& ActionClass : ItemActionsClass)
	{
		ULimenItemAction* Action = NewObject<ULimenItemAction>(this, ActionClass);
		Action->SetupAction(this);
		ItemActions.Push(Action);
	}
}

UTexture2D* ALimenItemBase::GetItemImage() const
{
	return ItemImage.LoadSynchronous();
}

const FText& ALimenItemBase::GetDisplayName() const
{
	return DisplayName;
}

const FText& ALimenItemBase::GetDescription() const
{
	return Description;
}

bool ALimenItemBase::ShouldSaveData() const
{
	return HasBeenInteracted();
}

bool ALimenItemBase::ShouldLoadData() const
{
	return bHasBeenLoaded;
}

void ALimenItemBase::DataLoaded()
{
	bHasBeenLoaded = true;
}

void ALimenItemBase::DataSaved()
{
}

TArray<ULimenItemAction*> ALimenItemBase::GetItemActions()
{
	for (ULimenItemAction* Action : ItemActions)
	{
		Action->ConditionalBeginDestroy();
	}
	ItemActions.Empty();
	
	for (const TSubclassOf<ULimenItemAction>& ActionClass : ItemActionsClass)
	{
		ULimenItemAction* Action = NewObject<ULimenItemAction>(this, ActionClass);
		Action->SetupAction(this);
		ItemActions.Push(Action);
	}
	
	return ItemActions;
}

void ALimenItemBase::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);
	RemoveFromGameplay();
}

void ALimenItemBase::InteractionStopped(AController* InController, APawn* InPawn)
{
	Super::InteractionStopped(InController, InPawn);
	
	// Hardcoded: Marked final because items should only need one interaction.
	// Remove the finality of this function in case that functionality is needed.
}
