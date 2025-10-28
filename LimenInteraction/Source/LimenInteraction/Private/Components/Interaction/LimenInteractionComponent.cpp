// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenInteractionComponent.h"

#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "GameFramework/Pawn.h"
#include "LogMacros/LimenInteractionLogMacros.h"
#include "LogMacros/LimenLogMacros.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Utils/LimenReplicationUtils.h"


bool ULimenInteractionComponent::IsInteractableActor(const AActor* InActor)
{
	return InActor && !InActor->GetComponentsByInterface(ULimenInteractableComponent::StaticClass()).IsEmpty();
}

bool ULimenInteractionComponent::IsInteractableComponent(const UActorComponent* InComponent)
{	
	return InComponent && InComponent->Implements<ULimenInteractableComponent>();
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

	if (GetOwner() && GetOwner()->HasAuthority())
	{
		UpdateInteraction(DeltaTime);

		if (CurrentInteractableInterface != PreviousInteractableInterface)
		{
			AActor* TempOwner = nullptr;
			if (CurrentInteractableInterface)
			{
				TempOwner = CurrentInteractableInterface->GetPrimitiveComponent()->GetOwner();
				check(TempOwner);
			}

			APawn* Pawn = Cast<APawn>(GetOwner());
			AController* Controller = Pawn ? Pawn->GetController() : nullptr;

			if (bIsInteracting)
			{
				StopInteraction();
			}

			Client_InteractableComponentHoveredChanged(CurrentInteractableInterface
				? CurrentInteractableInterface->GetPrimitiveComponent()
				: nullptr);


			if (CurrentInteractableInterface)
			{
				CurrentInteractableInterface->NotifyHover(Controller, Pawn);
			}
			if (PreviousInteractableInterface)
			{
				PreviousInteractableInterface->NotifyUnHover(Controller, Pawn);
			}

			OnInteractableHover.Broadcast(TempOwner, CurrentInteractableInterface);
		}

		PreviousInteractableInterface = CurrentInteractableInterface;
	}
}

void ULimenInteractionComponent::SetupInteractionParams(const FRepInteractionParams& InParams)
{
	check(GetOwner()->HasAuthority());
	InteractionParams = InParams;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, InteractionParams, this)
}

float ULimenInteractionComponent::GetInteractionRange() const
{
	return InteractionRange;
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
	if (SHOULD_PREDICT_NETWORK_EVENT_COMP)
	{
		InteractInternal();
	}

	Server_Interact();
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

	TArray<UActorComponent*> Components = SpecificInteractable->GetComponentsByInterface(ULimenInteractableComponent::StaticClass());
	auto* InteractableComponent = Cast<ILimenInteractableComponent>(Components[0]);
	check(InteractableComponent)

	InteractableComponent->Interact(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
	Multicast_Interact(InteractableComponent->GetPrimitiveComponent());
	return true;
}

bool ULimenInteractionComponent::Interact(UActorComponent* SpecificInteractableComponent)
{
	check(GetOwner()->HasAuthority())

	if (bIsInteracting)
	{
		return false;
	}
	
	if (!ensureAlways(IsInteractableComponent(SpecificInteractableComponent)))
	{
		LIMEN_LOG(LogLimenInteraction, Error, this, TEXT("Cannot interact with %s because it does not have an interactable component"), *SpecificInteractableComponent->GetOwner()->GetName());
		return false;
	}

	auto* InteractableComponent = Cast<ILimenInteractableComponent>(SpecificInteractableComponent);
	check(InteractableComponent)
	InteractableComponent->Interact(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
	Multicast_Interact(SpecificInteractableComponent);
	return true;
}

void ULimenInteractionComponent::StopInteraction()
{
	if (SHOULD_PREDICT_NETWORK_EVENT_COMP)
	{
		StopInteractInternal();
	}

	Server_StopInteract();
}

AController* ULimenInteractionComponent::GetInteractionController() const
{
	return InteractionParams.Controller.Get();
}

APawn* ULimenInteractionComponent::GetInteractionPawn() const
{
	return InteractionParams.Pawn.Get();
}

bool ULimenInteractionComponent::GetOwnerViewPoint(FVector& Start, FVector& End) const
{
	const auto* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn)
	{
		return false;
	}

	const auto* Controller = Cast<AController>(Pawn->GetController());
	if (!Controller)
	{
		return false;
	}
	
	FRotator EyesRotation;
	Controller->GetPlayerViewPoint(Start, EyesRotation);
	End = Start + EyesRotation.Vector() * InteractionRange;
	return true;
}

void ULimenInteractionComponent::SetupInteraction()
{	
	check(GetOwner()->HasAuthority());
}

void ULimenInteractionComponent::UpdateInteraction(const float DeltaTime)
{
}

void ULimenInteractionComponent::SetCurrentInteractableInterface(UActorComponent* InComponent)
{
	if (InComponent && InComponent->Implements<ULimenInteractableComponent>())
	{
		CurrentInteractableInterface = TScriptInterface<ILimenInteractableComponent>(InComponent);
	}
	else
	{
		CurrentInteractableInterface = nullptr;
	}
}

TScriptInterface<ILimenInteractableComponent> ULimenInteractionComponent::GetCurrentInteractableInterface() const
{
	return CurrentInteractableInterface;
}

UActorComponent* ULimenInteractionComponent::GetCurrentInteractableComponent() const
{
	return Cast<UActorComponent>(CurrentInteractableInterface.GetObject());
}

void ULimenInteractionComponent::Interacted(UActorComponent* Component)
{
	const TScriptInterface<ILimenInteractableComponent> Interface(Component);
	OnInteract.Broadcast(Component ? Component->GetOwner() : nullptr, Interface);
}

void ULimenInteractionComponent::InteractionStopped(UActorComponent* Component)
{
}

void ULimenInteractionComponent::Client_InteractableComponentHoveredChanged_Implementation(UActorComponent* Component)
{
	const TScriptInterface<ILimenInteractableComponent> Interface(Component);
	PreviousInteractableInterface = CurrentInteractableInterface;
	CurrentInteractableInterface = Interface;

	OnInteractableHover.Broadcast(Component ? Component->GetOwner() : nullptr, Interface);
}

void ULimenInteractionComponent::Server_Interact_Implementation()
{
	InteractInternal();
}

void ULimenInteractionComponent::Server_StopInteract_Implementation()
{
	StopInteractInternal();
}

void ULimenInteractionComponent::Multicast_Interact_Implementation(UActorComponent* Component)
{
	// Don't run on the client again since it predicted the interaction
	// If the local client owns the actor that contains this component, skip because it already predicted.
	if (IS_COMP_LOCAL_CLIENT_COPY) { return; }

	Interacted(Component);
}

void ULimenInteractionComponent::Multicast_StopInteract_Implementation(UActorComponent* Component)
{
	// Don't run on the client again since it predicted the interaction
	// If the local client owns the actor that contains this component, skip because it already predicted.
	if (IS_COMP_LOCAL_CLIENT_COPY) { return; }

	InteractionStopped(Component);
}

void ULimenInteractionComponent::InteractInternal()
{
	bIsInteracting = true;
	UpdateInteraction(0.f);
	
	if (CurrentInteractableInterface)
	{
		CurrentInteractableInterface->Interact(InteractionParams.Controller.Get(), InteractionParams.Pawn.Get());
		const AActor* ComponentOwner = CurrentInteractableInterface->GetPrimitiveComponent()->GetOwner();
		check(ComponentOwner);

		LIMEN_LOG(LogLimenInteraction, Log, this, TEXT("Interacted with: %s"), *ComponentOwner->GetName());
	}

	Multicast_Interact(GetCurrentInteractableComponent());
}

void ULimenInteractionComponent::StopInteractInternal()
{
	if (!bIsInteracting) { return; }
	
	if (PreviousInteractableInterface != nullptr)
	{
		PreviousInteractableInterface->StopInteraction(InteractionParams.Controller.Get(),
													   InteractionParams.Pawn.Get());
	}
	
	bIsInteracting = false;
	Multicast_StopInteract(PreviousInteractableInterface ? PreviousInteractableInterface->GetPrimitiveComponent() : nullptr);
}
