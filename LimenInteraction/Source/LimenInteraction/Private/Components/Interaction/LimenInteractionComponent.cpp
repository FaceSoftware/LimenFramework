// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenInteractionComponent.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Components/Interactable/LimenInteractableComponent.h"
#include "GameFramework/Pawn.h"
#include "LogMacros/LimenInteractionLogMacros.h"
#include "LogMacros/LimenLogMacros.h"
#include "Microsoft/AllowMicrosoftPlatformTypes.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utils/LimenReplicationUtils.h"


bool ULimenInteractionComponent::IsInteractableActor(const AActor* InActor)
{
	return InActor && !InActor->GetComponentsByInterface(ULimenInteractableComponent::StaticClass()).IsEmpty();
}

ULimenInteractionComponent::ULimenInteractionComponent(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	bAutoActivate = true;
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.f;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	
	InteractionRange = 0.f;
#if WITH_EDITORONLY_DATA
	bDebugMode = false;
#endif

	bIsInteracting = false;
}

void ULimenInteractionComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	constexpr FDoRepLifetimeParams Params(COND_OwnerOnly, REPNOTIFY_OnChanged, true);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InteractionParams, Params)
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, InteractionUpdateData, Params)
}

void ULimenInteractionComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if (InteractionRange < 0.f) { InteractionRange = 0.f; }
}

void ULimenInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		SetupInteraction();
	}
}

void ULimenInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UNLIKELY(!GetOwner())) { return; }

	CurrentInteractable.Reset();
	UpdateInteraction(DeltaTime);

	if (CurrentInteractable != PreviousInteractable)
	{
		AActor* TempOwner = nullptr;
		if (CurrentInteractable.IsValid())
		{
			TempOwner = CurrentInteractable->GetOwner();
			check(TempOwner)
		}
		
		if (UNLIKELY(bIsInteracting)) { StopInteraction(); }
		
		/// Replicate the state to the client, this way the state will rollback in case it detects anything not matching properly
		/// Change this on both client and server, so the OnRep is only called on the client if the value is different from the server
		{
			InteractionUpdateData.InteractableActor = TempOwner;
			InteractionUpdateData.ReplicatedGuid = CurrentInteractable.IsValid() ? CurrentInteractable->GetReplicatedGuid() : FGuid();
			MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionUpdateData, this)
		}
		
		APawn* Pawn = InteractionParams.Pawn.Get();
		AController* Controller = InteractionParams.Controller.Get();
		if (CurrentInteractable.IsValid())
		{
			CurrentInteractable->NotifyHover(Controller, Pawn);
		}
		if (PreviousInteractable.IsValid())
		{
			PreviousInteractable->NotifyUnHover(Controller, Pawn);
		}

		OnInteractableHover.Broadcast(TempOwner, CurrentInteractable.Get());
	}

	PreviousInteractable = CurrentInteractable;
}

void ULimenInteractionComponent::SetupInteractionParams(AController* Controller, APawn* Pawn)
{
	check(GetOwner()->HasAuthority());
	InteractionParams = FRepInteractionParams(Controller, Pawn);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionParams, this)
}

float ULimenInteractionComponent::GetInteractionRange() const
{
	return InteractionRange;
}

void ULimenInteractionComponent::SetInteractionRange(const float NewValue)
{
	InteractionRange = NewValue;
}

bool ULimenInteractionComponent::DebugMode() const
{
#if WITH_EDITORONLY_DATA
	return bDebugMode;
#else
	return false;
#endif
}

void ULimenInteractionComponent::RestartInteraction()
{
}

void ULimenInteractionComponent::Interact()
{
	NETWORK_PREDICTION(InteractInternal(), Server_Interact())
}

bool ULimenInteractionComponent::Interact(const AActor* SpecificInteractable)
{
	check(GetOwner()->HasAuthority())

	if (bIsInteracting)
	{
		return false;
	}

	if (!ensureAlways(IsInteractableActor(SpecificInteractable)))
	{
		LIMEN_LOG(LogLimenInteraction, Error, this, TEXT("Cannot interact with %s because it does not have an interactable component"), *SpecificInteractable->GetName());
		return false;
	}

	TArray<ULimenInteractableComponent*> Components;
	SpecificInteractable->GetComponents<ULimenInteractableComponent>(Components);
	check(!Components.IsEmpty()) // Validated by IsInteractableActor
	
	ULimenInteractableComponent* InteractableComponent = Components[0];
	check(InteractableComponent)
	if (InteractableComponent) { return false; }
	
	InteractableComponent->Interact(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
	Multicast_Interact(InteractableComponent);
	return true;
}

bool ULimenInteractionComponent::Interact(ULimenInteractableComponent* SpecificInteractableComponent)
{
	check(GetOwner()->HasAuthority())

	if (bIsInteracting) { return false; }
	if (!SpecificInteractableComponent) { return false; }
	
	SpecificInteractableComponent->Interact(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
	Multicast_Interact(SpecificInteractableComponent);
	return true;
}

void ULimenInteractionComponent::StopInteraction()
{
	NETWORK_PREDICTION(StopInteractInternal(), Server_StopInteract())
}

AController* ULimenInteractionComponent::GetInteractionController() const
{
	return InteractionParams.Controller.Get();
}

APawn* ULimenInteractionComponent::GetInteractionPawn() const
{
	return InteractionParams.Pawn.Get();
}

ULimenInteractableComponent* ULimenInteractionComponent::GetCurrentInteractableComponent() const
{
	return CurrentInteractable.Get();
}

bool ULimenInteractionComponent::GetOwnerViewPoint(FVector& Start, FVector& End) const
{
	TOptional<FRotator> EyesRotation;
	
	if (const auto* Pawn = Cast<APawn>(GetOwner()))
	{
		FRotator Temp;
		Pawn->GetActorEyesViewPoint(Start, Temp);
		EyesRotation = Temp;
	}
	else if (const auto* Controller = Cast<AController>(GetOwner()))
	{
		FRotator Temp;
		Controller->GetPlayerViewPoint(Start, Temp);
		EyesRotation = Temp;
	}

	if (!EyesRotation.IsSet()) return false;

	End = Start + EyesRotation.GetValue().Vector() * InteractionRange;
	return true;
}

void ULimenInteractionComponent::SetupInteraction()
{	
	check(GetOwner()->HasAuthority());
}

void ULimenInteractionComponent::SetCurrentInteractable(ULimenInteractableComponent* InComponent)
{
	CurrentInteractable = InComponent;
}

void ULimenInteractionComponent::Interacted(ULimenInteractableComponent* Component)
{
	OnInteract.Broadcast(Component ? Component->GetOwner() : nullptr, Component);
}

void ULimenInteractionComponent::InteractionStopped(ULimenInteractableComponent* Component)
{
}

void ULimenInteractionComponent::Server_Interact_Implementation()
{
	InteractInternal();
}

void ULimenInteractionComponent::Server_StopInteract_Implementation()
{
	StopInteractInternal();
}

void ULimenInteractionComponent::Multicast_Interact_Implementation(ULimenInteractableComponent* Component)
{
	Interacted(Component);
}

void ULimenInteractionComponent::Multicast_StopInteract_Implementation(ULimenInteractableComponent* Component)
{
	InteractionStopped(Component);
}

void ULimenInteractionComponent::OnRep_InteractionUpdateData()
{
	AActor* Actor = InteractionUpdateData.InteractableActor.Get();
	ULimenInteractableComponent* Component = nullptr;
	if (Actor)
	{
		const FGuid& Id = InteractionUpdateData.ReplicatedGuid;
		
		TArray<ULimenInteractableComponent*> Components;
		Actor->GetComponents<ULimenInteractableComponent>(Components);
		const auto* ClientComponent = Components.FindByPredicate([&Id] (const ULimenInteractableComponent* Component)
		{
			return Component && Component->GetReplicatedGuid() == Id;
		});
		
		checkf(ClientComponent, TEXT("Internal error, the component does not exist on the client. May be due to it's ReplicatedGuid not matching the client."))
		if (ClientComponent)
		{
			Component = *ClientComponent;
		}
	}
	OnInteractableHover.Broadcast(Actor, Component);
}

void ULimenInteractionComponent::InteractInternal()
{
	bIsInteracting = true;
	UpdateInteraction(0.f);
	
	if (CurrentInteractable.IsValid())
	{
		CurrentInteractable->Interact(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
		const AActor* ComponentOwner = CurrentInteractable->GetOwner();
		check(ComponentOwner);

		LIMEN_LOG(LogLimenInteraction, Log, this, TEXT("Interacted with: %s"), *ComponentOwner->GetName());
	}

	Multicast_Interact(GetCurrentInteractableComponent());
}

void ULimenInteractionComponent::StopInteractInternal()
{
	if (!bIsInteracting) { return; }
	
	if (PreviousInteractable != nullptr)
	{
		PreviousInteractable->StopInteraction(InteractionParams.Controller.Get(),
													   InteractionParams.Pawn.Get());
	}
	
	bIsInteracting = false;
	Multicast_StopInteract(PreviousInteractable.Get());
}
