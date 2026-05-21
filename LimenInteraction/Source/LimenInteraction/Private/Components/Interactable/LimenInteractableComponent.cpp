// Copyright Face Software. All Rights Reserved.


#include "Components/Interactable/LimenInteractableComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ULimenInteractableComponent::ULimenInteractableComponent() : Super()
{
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = false;
	SetGenerateOverlapEvents(true);
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	bAutoActivate = true;
}

void ULimenInteractableComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Params(COND_None, REPNOTIFY_OnChanged, true);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InteractionParams, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedGuid, Params)
}

void ULimenInteractableComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetCollisionProfileName(TEXT("OverlapAllDynamic"));
	
	if (GetOwner() && GetOwner()->HasAuthority())
	{
		ReplicatedGuid = FGuid::NewGuid();
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, ReplicatedGuid, this)
	}
}

void ULimenInteractableComponent::Activate(const bool bReset)
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

void ULimenInteractableComponent::Deactivate()
{
	Super::Deactivate();

	CachedCollisionResponse = GetCollisionResponseToChannels();
	CachedCollisionEnabledType = GetCollisionEnabled();
	
	SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void ULimenInteractableComponent::NotifyHover(AController* Controller, APawn* Pawn)
{
	OnHoverStart.Broadcast(Controller, Pawn);
}

void ULimenInteractableComponent::NotifyUnHover(AController* Controller, APawn* Pawn)
{
	OnHoverEnd.Broadcast(Controller, Pawn);
}

bool ULimenInteractableComponent::AllowPhysicsInteraction() const
{
	return false;
}

bool ULimenInteractableComponent::IsBeingInteracted() const
{
	return InteractionParams.bIsBeingInteracted;
}

const FGuid& ULimenInteractableComponent::GetReplicatedGuid() const
{
	return ReplicatedGuid;
}

void ULimenInteractableComponent::Interact(AController* InController, APawn* InPawn)
{
	if (GetOwner()->HasAuthority())
	{
		InteractionParams = {InController, InPawn, true};
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionParams, this)
	}

	OnInteract.Broadcast(InController, InPawn);
}

void ULimenInteractableComponent::StopInteraction(AController* InController, APawn* InPawn)
{
	if (GetOwner()->HasAuthority())
	{
		InteractionParams = {InController, InPawn, false};
		MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionParams, this)
	}

	OnInteractionStopped.Broadcast(InController, InPawn);
}

void ULimenInteractableComponent::OnRep_IsBeingInteracted()
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
