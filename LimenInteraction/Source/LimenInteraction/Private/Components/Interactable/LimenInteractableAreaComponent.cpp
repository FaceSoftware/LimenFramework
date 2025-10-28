// Copyright Face Software. All Rights Reserved.


#include "Components/Interactable/LimenInteractableAreaComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ULimenInteractableAreaComponent::ULimenInteractableAreaComponent() : Super()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetGenerateOverlapEvents(true);
}

void ULimenInteractableAreaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params(COND_None, REPNOTIFY_OnChanged, true);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InteractionParams, Params)
}

void ULimenInteractableAreaComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	SetCollisionProfileName(TEXT("OverlapAllDynamic"));
}

void ULimenInteractableAreaComponent::Activate(const bool bReset)
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

bool ULimenInteractableAreaComponent::AllowPhysicsInteraction() const
{
	return false;
}

bool ULimenInteractableAreaComponent::IsBeingInteracted() const
{
	return InteractionParams.bIsBeingInteracted;
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
	if (GetOwner()->HasAuthority())
	{
		InteractionParams = {InController, InPawn, true};
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionParams, this)
	}

	OnInteract.Broadcast(InController, InPawn);
	BP_OnInteract.Broadcast(InController, InPawn);
}

void ULimenInteractableAreaComponent::StopInteraction(AController* InController, APawn* InPawn)
{
	if (GetOwner()->HasAuthority())
	{
		InteractionParams = {InController, InPawn, false};
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionParams, this)
	}

	OnInteractionStopped.Broadcast(InController, InPawn);
	BP_OnInteractionStopped.Broadcast(InController, InPawn);
}

void ULimenInteractableAreaComponent::OnRep_IsBeingInteracted()
{
	if (InteractionParams.bIsBeingInteracted)
	{
		Interact(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
	}
	else
	{
		StopInteraction(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
	}
}
