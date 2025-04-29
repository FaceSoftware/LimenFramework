// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LimenGameplayActor.generated.h"

class ULimenObjectiveComponent;
class ULimenCollectableComponent;


UENUM(BlueprintType)
enum class ELimenGameplayActorState : uint8
{
	InGameplay,
	InGameplayWithCollisionDisabled,
	OutOfGameplay,
};

/**
 * @brief Represents a specialized actor used in the gameplay layer.
 *
 * ALimenGameplayActor serves as a base class for actors that play a significant role in the gameplay logic.
 * It provides foundational functionality required for actors to interact with the game environment,
 * enabling characteristics such as game-state-dependent behaviors, replication, and interaction handling.
 *
 * The class is designed to be extended to accommodate specific gameplay implementations, offering flexibility
 * to define unique features and overrides tailored to game-specific needs.
 *
 * Additionally, it supports systems like collision management, game event handling, and network-related functionality,
 * making it a suitable base class for a wide variety of gameplay-related actors.
 */
UCLASS(Abstract, NotBlueprintable)
class LIMENCORE_API ALimenGameplayActor : public AActor
{
	GENERATED_BODY()
	
public:
	explicit ALimenGameplayActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/**
	 * @brief Removes the actor from gameplay by disabling its key functionalities.
	 *
	 * This method hides the actor, disables its collision, and marks it as removed from gameplay.
	 * Additionally, it disables network replication for the actor.
	 * This can be used to handle cases where the actor is no longer active in the gameplay or intended to
	 * be removed logically while maintaining its instance in the world for potential reactivation or other purposes.
	 *
	 * Note that this method ensures that further calls have no effect if the actor has already been
	 * marked as removed from gameplay.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay Actor")
	virtual void RemoveFromGameplay();
	/**
	 * @brief Adds the actor back into gameplay by enabling its key functionalities.
	 *
	 * This method unhides the actor, enables its collision, and sets it to replicate over the network.
	 * The actor's state is updated to indicate it is no longer removed from gameplay.
	 *
	 * Typically, this is used to reactivate actors that were previously removed from gameplay
	 * using RemoveFromGameplay(), allowing them to participate fully in the game world again.
	 *
	 * The method has no effect if the actor is already active in gameplay.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay Actor")
	virtual void AddToGameplay(const bool bEnableCollision = true);
	/**
	 * @brief Checks whether the actor is marked as removed from gameplay.
	 *
	 * This method returns the current state of the actor, indicating whether it has been
	 * marked as removed from gameplay through a call to RemoveFromGameplay().
	 *
	 * @return true if the actor is removed from gameplay, false otherwise.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay Actor")
	bool IsRemovedFromGameplay() const;

protected:
	
private:
	UPROPERTY(ReplicatedUsing=OnRep_GameplayState)
	ELimenGameplayActorState GameplayState;

	UFUNCTION()
	void OnRep_GameplayState();
};