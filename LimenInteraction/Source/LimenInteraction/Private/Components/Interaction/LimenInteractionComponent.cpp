// Copyright Face Software. All Rights Reserved.


#include "Components/Interaction/LimenInteractionComponent.h"

#include "Components/Interactable/LimenInteractableAreaComponent.h"
#include "LogMacros/LimenInteractionLogMacros.h"
#include "LogMacros/LimenLogMacros.h"

ULimenInteractionComponent::ULimenInteractionComponent(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
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

	SetupInteraction();
}

void ULimenInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!IsActive())
	{
		return;
	}
	
	UpdateInteraction(DeltaTime);
	if (CurrentInteractableInterface != PreviousInteractableInterface)
	{
		AActor* TempOwner = nullptr;
		if (CurrentInteractableInterface)
		{
			TempOwner = CurrentInteractableInterface->GetPrimitiveComponent()->GetOwner();
			check(TempOwner);
		}

		if (bIsInteracting)
		{
			APawn* Pawn = Cast<APawn>(GetOwner());
			AController* Controller = Pawn->GetController();
			StopInteraction(Controller, Pawn);
		}
		
		OnInteractableHover.Broadcast(TempOwner, CurrentInteractableInterface);
	}

	PreviousInteractableInterface = CurrentInteractableInterface;
}

bool ULimenInteractionComponent::Interact(AController* InController, APawn* InPawn)
{
	if (bIsInteracting)
	{
		return false;
	}
	
	bIsInteracting = true;
	UpdateInteraction(0.f);
	
	if (CurrentInteractableInterface)
	{
		CurrentInteractableInterface->Interact(InController, InPawn);
		AActor* ComponentOwner = CurrentInteractableInterface->GetPrimitiveComponent()->GetOwner();
		check(ComponentOwner);
		
		OnInteract.Broadcast(ComponentOwner, CurrentInteractableInterface);
		LIMEN_LOG(LogLimenInteraction, Log, this, "Interacted with: %s", *ComponentOwner->GetName());
		return true;
	}
	
	return false;
}

bool ULimenInteractionComponent::Interact(AController* InController, APawn* InPawn, AActor* SpecificInteractable)
{
	TArray<UActorComponent*> Components = SpecificInteractable->GetComponentsByInterface(ULimenInteractableComponent::StaticClass());
	if (Components.IsEmpty())
	{
		LIMEN_LOG(LogLimenInteraction, Error, this, "Cannot interact with %s because it does not have an interactable component", *SpecificInteractable->GetName());
		return false;
	}
	
	ILimenInteractableComponent* InteractableComponent = Cast<ILimenInteractableComponent>(Components[0]);
	check(InteractableComponent)
	
	TScriptInterface<ILimenInteractableComponent> InteractableInterface;
	InteractableInterface.SetInterface(InteractableComponent);
	InteractableInterface.SetObject(Components[0]);
	OnInteract.Broadcast(SpecificInteractable, InteractableInterface);
	return true;
}

bool ULimenInteractionComponent::Interact(AController* InController, APawn* InPawn, UActorComponent* SpecificInteractableComponent)
{
	if (!SpecificInteractableComponent->Implements<ULimenInteractionComponent>())
	{
		LIMEN_LOG(LogLimenInteraction, Error, this, "Cannot interact with %s because it does not have an interactable component", *SpecificInteractableComponent->GetOwner()->GetName());
		return false;
	}

	auto* InteractableComponent = Cast<ILimenInteractableComponent>(SpecificInteractableComponent);
	check(InteractableComponent)
	InteractableComponent->Interact(InController, InPawn);
	OnInteract.Broadcast(SpecificInteractableComponent->GetOwner(), CurrentInteractableInterface);
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
}

bool ULimenInteractionComponent::GetOwnerLineOfSightStartAndEndVectors(FVector& Start, FVector& End) const
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
