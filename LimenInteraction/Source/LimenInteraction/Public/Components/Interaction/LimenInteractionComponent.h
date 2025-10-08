// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenInteractionComponent.generated.h"


class AController;
class ALimenInteractable;
class ULimenInteractableComponent;
class ILimenInteractableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractableHover, AActor*, InteractableActor, const TScriptInterface<ILimenInteractableComponent>&, Interactable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBPInteractionDelegate, AActor*, InteractableActor, const TScriptInterface<ILimenInteractableComponent>&, Interactable);

/**
 * @brief Abstract class that provides interaction logic for actors and components.
 *
 * ULimenInteractionComponent serves as a base class for defining interaction mechanics,
 * including hover and interact events, as well as interaction-specific logic such as handling line of sight
 * and interaction ranges.
 * It supports integration with components that implement the ILimenInteractableComponent interface.
 */
UCLASS(Abstract)
class LIMENINTERACTION_API ULimenInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief Delegate event triggered when an interactable component enters or exits hover state.
	 *
	 * This event, bound via Blueprint or C++, is broadcast when the interaction system detects
	 * that a hover event has occurred.
	 * This includes scenarios when an interactable component
	 * begins or ends hovering, enabling custom logic to be executed at these points.
	 *
	 * The event passes information about the involved interactable, providing hooks for initiating
	 * or ceasing hover-based interactions, such as highlighting UI elements or playing sound effects.
	 */
	UPROPERTY(BlueprintAssignable)
	FInteractableHover OnInteractableHover;
	/**
	 * @brief Delegate event triggered when an interaction occurs.
	 *
	 * The OnInteract event is fired whenever an interaction is successfully performed with an interactable object
	 * or component.
	 * This delegate can be used to define custom logic in both C++ and Blueprint when a player
	 * or system interacts with a valid target.
	 *
	 * Useful for triggering gameplay events, UI updates, or audiovisual feedback based on interaction actions.
	 *
	 * The event passes relevant information, including the owner actor of the interactable and the associated
	 * interactable component interface.
	 */
	UPROPERTY(BlueprintAssignable)
	FInteractableHover OnInteract;

	/**
	 * @brief Determines if the specified actor is interactable.
	 *
	 * This method checks if the provided actor contains any component that implements the
	 * ILimenInteractableComponent interface, indicating that the actor is capable of interaction.
	 *
	 * @param InActor Pointer to the actor being checked for interactability.
	 * @return true if the actor contains an interactable component, false otherwise.
	 */
	static bool IsInteractableActor(const AActor* InActor);
	/**
	 * @brief Determines if the specified component is interactable.
	 *
	 * This method checks whether the given component implements the ULimenInteractableComponent
	 * interface and is valid for interaction purposes.
	 *
	 * @param InComponent The actor component to be checked.
	 * @return true if the component is interactable, false otherwise.
	 */
	static bool IsInteractableComponent(const UActorComponent* InComponent);

	explicit ULimenInteractionComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void OnComponentCreated() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/**
	 * @brief Retrieves the interaction range for this component.
	 *
	 * This method returns the distance at which this component can interact with other objects.
	 *
	 * @return The interaction range as a floating-point value.
	 */
	float GetInteractionRange() const;
	/**
	 * 
	 * @return 
	 */
	bool DebugMode() const;

	/**
	 * @brief Virtual function to restart interaction logic.
	 *
	 * This function enforces derived classes to implement their own handling of restarting the interaction process.
	 * It acts as an entry point to reset or reinitialize interaction-related states or behaviors,
	 * ensuring consistency across custom implementations.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	virtual void RestartInteraction();

	/**
	 * @brief Initiates the interaction logic with the current interactable object.
	 *
	 * This method verifies the interaction state and triggers the interaction process if applicable.
	 * It notifies the interactable interface and broadcasts relevant interaction events.
	 *
	 * @param InController The controller initiating the interaction.
	 * @param InPawn The pawn associated with the interaction.
	 * @return True if the interaction is successfully initiated, false otherwise.
	 */
	virtual bool Interact(AController* InController, APawn* InPawn);
	/**
	 * @brief Attempts to perform an interaction with a specific interactable actor.
	 *
	 * This method checks if the specified actor has a valid interactable component implementing
	 * the ILimenInteractableComponent interface.
	 * If a valid interactable component is found, it triggers the OnInteract delegate event,
	 * passing relevant interaction information.
	 * The interaction logic can be customized by overriding or extending this method in derived classes.
	 *
	 * @param InController The controller initiating the interaction, typically representing the player or AI.
	 * @param InPawn The pawn associated with the controller, providing the perspective or ownership context for the interaction.
	 * @param SpecificInteractable The specific actor being targeted for interaction.
	 * @return True if the interaction was successfully performed, otherwise false.
	 */
	virtual bool Interact(AController* InController, APawn* InPawn, const AActor* SpecificInteractable);
	/**
	 * @brief Attempts to perform an interaction with a specified interactable component.
	 *
	 * Executes an interaction with the provided SpecificInteractableComponent
	 * if it implements the ILimenInteractableComponent interface.
	 * This method triggers the OnInteract delegate upon successful interaction
	 * and returns a boolean status based on the result.
	 *
	 * @param InController The controller associated with the initiating actor or player.
	 * @param InPawn The pawn representing the entity attempting the interaction.
	 * @param SpecificInteractableComponent The specific component to interact with, expected to implement the ILimenInteractableComponent interface.
	 * @return true if the interaction is successful, false otherwise (e.g.,
	 * if the component does not implement the required interface).
	 */
	virtual bool Interact(AController* InController, APawn* InPawn, UActorComponent* SpecificInteractableComponent);

	/**
	 * @brief Stops the current interaction process by notifying the interactable component and updating interaction states.
	 *
	 * This method terminates an active interaction session initiated by the specified controller and pawn.
	 * It ensures that the stop logic is executed for the previous interactable component, if applicable,
	 * and resets the interaction state of the component.
	 *
	 * @param InController The controller associated with the interaction, usually representing the player or AI controlling the pawn.
	 * @param InPawn The pawn involved in the interaction process, typically the one under the control of the provided controller.
	 */
	virtual void StopInteraction(AController* InController, APawn* InPawn);

protected:
#if WITH_EDITORONLY_DATA

	/**
	 * @brief Toggles debug mode for interaction-related components.
	 *
	 * When enabled, bDebugMode activates additional debugging tools or visualizations
	 * to assist developers in diagnosing and testing interaction logic.
	 * This can include features such as displaying interaction ranges, visibility checks, and other
	 * diagnostics specific to the interaction system under the "Limen" category.
	 */
	UPROPERTY(EditAnywhere, Category="Limen")
	bool bDebugMode;

#endif

	/**
	 * @brief Defines the maximum range within which interactions can occur.
	 *
	 * The InteractionRange determines the effective distance from which the actor or component
	 * can detect and interact with interactable objects.
	 * It allows customization for different gameplay interaction scenarios, ensuring appropriate proximity thresholds.
	 */
	UPROPERTY(EditAnywhere, Category="Limen")
	float InteractionRange;

	/**
	 * @brief Retrieves the view point start and end vectors for the component's owning actor.
	 *
	 * This method calculates the starting and ending points of the line of sight based on the view point
	 * of the owner actor's controller, typically used for interaction logic.
	 * If the owner is not a pawn or does not have a valid controller, the method will fail.
	 *
	 * @param Start Output parameter to store the starting point of the line of sight.
	 * @param End Output parameter to store the ending point of the line of sight.
	 * @return True if the line-of-sight vectors were successfully computed, false otherwise.
	 */
	bool GetOwnerViewPoint(FVector& Start, FVector& End) const;

	/**
	 * @brief Pure virtual function responsible for initializing interaction mechanics.
	 *
	 * This function must be implemented by derived classes to define the specific setup logic
	 * required for enabling interactions.
	 * It ensures the proper configuration of interaction-related
	 * features such as input bindings, event listeners, or other mechanics necessary for interaction.
	 */
	virtual void SetupInteraction();
	/**
	 * @brief Updates interaction logic for the deriving class.
	 *
	 * This function is intended to be overridden by child classes
	 * to define the specific interaction update behavior executed every frame.
	 * It processes interaction-related updates, optionally dependent on DeltaTime.
	 *
	 * @param DeltaTime The time elapsed since the last frame, used for frame-dependent calculations.
	 */
	virtual void UpdateInteraction(const float DeltaTime);

	void SetCurrentInteractableInterface(UActorComponent* InComponent);
	FORCEINLINE TScriptInterface<ILimenInteractableComponent> GetCurrentInteractableInterface() const;
	FORCEINLINE UActorComponent* GetCurrentInteractableComponent() const;

	virtual void Interacted(UActorComponent* Component);
	virtual void InteractionStopped(UActorComponent* Component);

private:
	UPROPERTY(BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TScriptInterface<ILimenInteractableComponent> CurrentInteractableInterface;
	TScriptInterface<ILimenInteractableComponent> PreviousInteractableInterface;

	bool bIsInteracting;

	UFUNCTION(Client, Reliable)
	void Client_InteractableComponentHoveredChanged(UActorComponent* Component);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Interacted(UActorComponent* Component);
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_InteractionStopped(UActorComponent* Component);
};
