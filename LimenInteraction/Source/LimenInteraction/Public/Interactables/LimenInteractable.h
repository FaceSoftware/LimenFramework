// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "Interfaces/LimenInteractableComponent.h"
#include "LimenInteractable.generated.h"


class ULimenInteractableComponent;

DECLARE_MULTICAST_DELEGATE_TwoParams(FInteractableDelegate, AController* /* Controller */, APawn* /* Pawn */)

UCLASS(BlueprintType, Blueprintable)
class LIMENINTERACTION_API ALimenInteractable : public ALimenGameplayActor
{
	GENERATED_BODY()

public:
	FInteractableDelegate OnInteract;
	FInteractableDelegate OnInteractionStopped;
	
	explicit ALimenInteractable(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	bool HasBeenInteracted() const;
	uint32 GetInteractionCount() const;

	template<typename ComponentClass>
	TArray<ComponentClass*> GetInteractableComponents() const
	{
		TArray<UActorComponent*> InteractableComponents = GetComponentsByInterface(ULimenInteractableComponent::StaticClass());

		TArray<ComponentClass*> OutComponents;
		OutComponents.Reserve(InteractableComponents.Num());
		for (UActorComponent* InteractableComponent : InteractableComponents)
		{
			checkf(InteractableComponent->Implements<ULimenInteractableComponent>(), TEXT("Interactable components must inherit ILimenInteractableComponent interface"));
			OutComponents.Push(CastChecked<ComponentClass>(InteractableComponent));
		}
		return OutComponents;
	}

	TArray<ILimenInteractableComponent*> GetInteractableComponents() const;

	UFUNCTION(BlueprintCallable)
	bool IsBeingContinuouslyInteracted() const;
	
protected:
	/**
	 * @brief Simulates an interaction.
	 * Useful after loading to simulate previous player interactions.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void SimulateInteraction();
	
	UFUNCTION(BlueprintImplementableEvent, DisplayName="On Interact")
	void BP_OnInteract(AController* InController, APawn* InPawn);
	virtual void Interact(AController* InController, APawn* InPawn);
	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastRPC_Interact(AController* InController, APawn* InPawn);

	UFUNCTION(BlueprintImplementableEvent, Category="Limne|Interaction")
	void BP_OnInteractionStopped(AController* InController, APawn* InPawn);
	virtual void InteractionStopped(AController* InController, APawn* InPawn);
	
private:
	void OnInteract_Internal(AController* InController, APawn* InPawn);
	void OnInteractionStopped_Internal(AController* InController, APawn* InPawn);

	UPROPERTY(SaveGame)
	bool bWasInteracted;
	UPROPERTY(SaveGame)
	int32 InteractionCount;
	
	bool bIsBeingContinuouslyInteracted;
};
