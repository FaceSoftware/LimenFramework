// Copyright Face Software. All Rights Reserved.


#include "Items/LimenItemBase.h"

#include "TextureResource.h"
#include "TimerManager.h"
#include "Components/PrimitiveComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Components/Interactable/LimenInteractableComponent.h"
#include "Engine/Engine.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/Pawn.h"
#include "ItemActions/LimenItemAction.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ALimenItemBase::ALimenItemBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	ItemQuantity = 1;
}

void ALimenItemBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PickUpState, Params);
}

void ALimenItemBase::BeginPlay()
{
	Super::BeginPlay();
	
	for (const TSubclassOf<ULimenItemAction>& ActionClass : ItemActionsClass)
	{
		if (!ensureAlwaysMsgf(ActionClass != nullptr, TEXT("This item contains an invalid item action. Please remove it to prevent unnecessary overhead!!")))
		{
			continue;
		}
		
		ULimenItemAction* Action = NewObject<ULimenItemAction>(this, ActionClass);
		Action->SetupAction(this);
		ItemActions.Push(TStrongObjectPtr(Action));
	}
}

void ALimenItemBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ItemActions.Empty();
}

UStaticMeshComponent* ALimenItemBase::GetStaticMesh_Implementation() const
{
	return nullptr;
}

USkeletalMeshComponent* ALimenItemBase::GetSkeletalMesh_Implementation() const
{
	return nullptr;
}

UMeshComponent* ALimenItemBase::GetMesh_Implementation() const
{
	if (UMeshComponent* StaticMesh = GetStaticMesh()) { return StaticMesh; }
	return GetSkeletalMesh();
}

void ALimenItemBase::OnRep_PickUpState()
{
	if (PickUpState.bIsDropped)
	{
		Dropped(PickUpState.InController.Get(), PickUpState.InPawn.Get());
		OnDropped.Broadcast();
	}
	else
	{
		PickedUp(PickUpState.InController.Get(), PickUpState.InPawn.Get());
		OnPickedUp.Broadcast();
	}
}

UTexture* ALimenItemBase::GetItemImage() const
{
	return ItemImage.Get();
}

const FText& ALimenItemBase::GetDisplayName() const
{
	return DisplayName;
}

const FText& ALimenItemBase::GetDescription() const
{
	return Description;
}

TArray<ULimenItemAction*> ALimenItemBase::GetItemActions() const
{
	TArray<ULimenItemAction*> Out;
	Out.Reserve(ItemActions.Num());
	
	for (auto& Action : ItemActions)
	{
		Out.Push(Action.Get());
	}
	
	return Out;
}

AActor* ALimenItemBase::GetActor()
{
	return this;
}

void ALimenItemBase::PickUp(AController* InController, APawn* InPawn)
{
	check(HasAuthority())

	PickUpState.InController = InController;
	PickUpState.InPawn = InPawn;
	PickUpState.bIsDropped = false;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PickUpState, this)

	PickedUp(InController, InPawn);
	OnPickedUp.Broadcast();
}

void ALimenItemBase::Drop(AController* InController, APawn* InPawn)
{
	check(HasAuthority())

	PickUpState.InController = InController;
	PickUpState.InPawn = InPawn;
	PickUpState.bIsDropped = true;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PickUpState, this)

	Dropped(InController, InPawn);
	OnDropped.Broadcast();
}

bool ALimenItemBase::IsDropped() const
{
	return PickUpState.bIsDropped;
}

int32 ALimenItemBase::GetItemQuantity() const
{
	return ItemQuantity;
}

void ALimenItemBase::SetItemQuantity(const int32 InItemQuantity)
{
	ItemQuantity = InItemQuantity;
}

void ALimenItemBase::Interact(AController* InController, APawn* InPawn)
{
	Super::Interact(InController, InPawn);
}

void ALimenItemBase::InteractionStopped(AController* InController, APawn* InPawn)
{
	Super::InteractionStopped(InController, InPawn);
	
	// Hardcoded: Marked final because items should only need one interaction.
	// Remove the finality of this function in case that functionality is needed.
}

void ALimenItemBase::PickedUp(AController* InController, APawn* InPawn)
{
	if (HasAuthority()) { SetOwner(InPawn); }

	for (ULimenInteractableComponent*& InteractableComponent : GetInteractableComponents<ULimenInteractableComponent>())
	{
		InteractableComponent->Deactivate();
	}

	if (FMath::IsNearlyZero(PickupAnimationTime))
	{
		RemoveFromGameplay();
	}
	else
	{
		PickUpAnimation(PickupAnimationTime);
		GetWorld()->GetTimerManager().SetTimer(InteractAnimationTimerHandle,
											   this, &ThisClass::RemoveFromGameplay,
											   PickupAnimationTime, false);
	}
}

void ALimenItemBase::Dropped(AController* InController, APawn* InPawn)
{
	if (HasAuthority()) { SetOwner(nullptr); }

	for (ULimenInteractableComponent*& InteractableComponent : GetInteractableComponents<ULimenInteractableComponent>())
	{
		InteractableComponent->Activate(true);
	}

	AddToGameplay();
}

void ALimenItemBase::PickUpAnimation(const float AnimationTime)
{
}
