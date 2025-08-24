// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenInteractionComponent.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "GameFramework/Pawn.h"
#include "LogMacros/LimenInteractionLogMacros.h"
#include "LogMacros/LimenLogMacros.h"

ULimenInteractionComponent::ULimenInteractionComponent(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	bAutoActivate = true;
	
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.03125f;
	PrimaryComponentTick.bTickEvenWhenPaused = false;
	
	InteractionRange = 0.f;
#if WITH_EDITORONLY_DATA
	bDebugMode = false;
#endif

	bIsInteracting = false;
}

void ULimenInteractionComponent::OnComponentCreated()
{
	Super::OnComponentCreated();

	if (InteractionRange < 0.f)
	{
		InteractionRange = 0.f;
	}
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
			AController* Controller = Pawn->GetController();

			if (bIsInteracting)
			{
				StopInteraction(Controller, Pawn);
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

bool ULimenInteractionComponent::Interact(AController* InController, APawn* InPawn)
{
	check(GetOwner()->HasAuthority())
	// check(!bIsInteracting)
	// if (bIsInteracting)
	// {
	// 	return false;
	// }
	
	bIsInteracting = true;
	UpdateInteraction(0.f);
	
	if (CurrentInteractableInterface)
	{
		CurrentInteractableInterface->Interact(InController, InPawn);
		const AActor* ComponentOwner = CurrentInteractableInterface->GetPrimitiveComponent()->GetOwner();
		check(ComponentOwner);
		
		Multicast_Interacted(CurrentInteractableInterface->GetPrimitiveComponent());

		LIMEN_LOG(LogLimenInteraction, Log, this, TEXT("Interacted with: %s"), *ComponentOwner->GetName());
		return true;
	}

	LIMEN_LOG(LogLimenInteraction, Log, this, TEXT("Interacted with air..."));

	// Todo: Will need cooldown on network
	Multicast_Interacted(nullptr);
	return false;
}

bool ULimenInteractionComponent::Interact(AController* InController, APawn* InPawn, const AActor* SpecificInteractable)
{
	check(GetOwner()->HasAuthority())

	if (bIsInteracting)
	{
		return false;
	}
	
	TArray<UActorComponent*> Components = SpecificInteractable->GetComponentsByInterface(ULimenInteractableComponent::StaticClass());
	if (!ensureAlways(!Components.IsEmpty()))
	{
		LIMEN_LOG(LogLimenInteraction, Error, this, TEXT("Cannot interact with %s because it does not have an interactable component"), *SpecificInteractable->GetName());
		return false;
	}

	auto* InteractableComponent = Cast<ILimenInteractableComponent>(Components[0]);
	check(InteractableComponent)

	InteractableComponent->Interact(InController, InPawn);
	Multicast_Interacted(InteractableComponent->GetPrimitiveComponent());
	return true;
}

bool ULimenInteractionComponent::Interact(AController* InController, APawn* InPawn, UActorComponent* SpecificInteractableComponent)
{
	check(GetOwner()->HasAuthority())

	if (bIsInteracting)
	{
		return false;
	}
	
	if (!ensureAlways(SpecificInteractableComponent->Implements<ULimenInteractionComponent>()))
	{
		LIMEN_LOG(LogLimenInteraction, Error, this, TEXT("Cannot interact with %s because it does not have an interactable component"), *SpecificInteractableComponent->GetOwner()->GetName());
		return false;
	}

	auto* InteractableComponent = Cast<ILimenInteractableComponent>(SpecificInteractableComponent);
	check(InteractableComponent)
	InteractableComponent->Interact(InController, InPawn);
	Multicast_Interacted(SpecificInteractableComponent);
	return true;
}

void ULimenInteractionComponent::StopInteraction(AController* InController, APawn* InPawn)
{
	if (!bIsInteracting)
	{
		return;
	}
	
	if (PreviousInteractableInterface != nullptr)
	{
		PreviousInteractableInterface->StopInteraction(InController, InPawn);
	}
	
	bIsInteracting = false;
	UPrimitiveComponent* Component = PreviousInteractableInterface
								   ? PreviousInteractableInterface->GetPrimitiveComponent()
								   : nullptr;

	Multicast_InteractionStopped(Component);
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
	check(GetOwner()->HasAuthority());
}

void ULimenInteractionComponent::SetCurrentInteractableInterface(UActorComponent* InComponent)
{
	CurrentInteractableInterface = TScriptInterface<ILimenInteractableComponent>(InComponent);
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

void ULimenInteractionComponent::Multicast_Interacted_Implementation(UActorComponent* Component)
{
	Interacted(Component);
}

void ULimenInteractionComponent::Multicast_InteractionStopped_Implementation(UActorComponent* Component)
{
	InteractionStopped(Component);
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

bool ULimenInteractionComponent::IsInteractableActor(const AActor* InActor)
{
	if (!IsValid(InActor))
	{
		return false;
	}
	
	for (const UActorComponent* Component : InActor->GetComponents())
	{
		if (Component->Implements<ULimenInteractableComponent>())
		{
			return true;
		}
	}

	return false;
}

bool ULimenInteractionComponent::IsInteractableComponent(const UActorComponent* InComponent)
{
	if (!IsValid(InComponent))
	{
		return false;
	}
	
	return InComponent->Implements<ULimenInteractableComponent>();
}
