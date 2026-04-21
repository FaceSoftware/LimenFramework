// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "LimenGameplayManager.generated.h"


class AGameModeBase;

/**
 * Represents a gameplay manager base class designed to manage gameplay logic and lifecycle interactions.
 *
 * This class provides a framework for creating and managing gameplay logic associated with a specific
 * game mode or context. It integrates core functionality like initialization, instance management,
 * and interaction with associated game modes. Derived classes should add game-specific logic,
 * using the provided virtual functions and Blueprint extension points.
 */
UCLASS(Abstract, Blueprintable)
class LIMENCORE_API ALimenGameplayManager : public AInfo
{
	GENERATED_BODY()

public:
	/**
	 * Retrieves a gameplay manager instance of the specified class from the given world context.
	 *
	 * This function searches the world associated with the provided context object for an actor
	 * of the specified gameplay manager class and returns it.
	 * If no matching instance is found, the function will return nullptr.
	 *
	 * @param WorldContextObject The context object used to derive the world in which to search for the manager.
	 *                           Typically, this is a valid UObject with access to the game world.
	 * @param ManagerClass The class type of the gameplay manager to retrieve. Only one instance of this class
	 *                     can exist in the game world.
	 * @return A pointer to the gameplay manager instance of the specified class if found, otherwise nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Manager", meta=(WorldContext="WorldContextObject", DeterminesOutputType="ManagerClass"))
	static ALimenGameplayManager* GetGameplayManager(UObject* WorldContextObject, TSubclassOf<ALimenGameplayManager> ManagerClass);

	/**
	 * Retrieves an instance of the specified gameplay manager class for the given world context.
	 * This method ensures the provided manager class is derived from ALimenGameplayManager
	 * and returns the corresponding instance if found.
	 *
	 * @param WorldContextObject The context object used to retrieve the world reference.
	 * Typically, this is an object that exists in the same world as the desired manager.
	 * @return A pointer to the specified gameplay manager class instance if it exists,
	 * or nullptr if no instance is found.
	 */
	template<typename ManagerClass>
	static ManagerClass* GetGameplayManager(UObject* WorldContextObject)
	{
		static_assert(std::is_base_of_v<ALimenGameplayManager, ManagerClass>);
		return Cast<ManagerClass>(GetGameplayManager(WorldContextObject, ManagerClass::StaticClass()));
	}
	
	explicit ALimenGameplayManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	/**
	 * Binds the specified GameMode instance to this gameplay manager.
	 * This method associates the provided GameModeBase instance with the manager,
	 * allowing the manager to interact with and manage gameplay logic related to the GameMode.
	 *
	 * @param InGameMode A pointer to the GameModeBase instance to bind to this manager.
	 *                   This can be used to access or link the manager's functionality with the game mode logic.
	 */
	void BindGameMode(AGameModeBase* InGameMode);

	/**
	 * Checks whether the gameplay manager has been initialized.
	 *
	 * This method verifies if the manager has completed its initialization process
	 * and is ready to perform its functionality. It can be used to ensure preconditions
	 * are met before interacting with or relying on the manager's logic.
	 *
	 * @return True if the manager has been initialized, otherwise false.
	 */
	bool HasInitialized() const;
	

protected:
	/**
	 * Retrieves the GameMode associated with this Gameplay Manager.
	 * This function returns a pointer to the GameModeBase instance that is currently associated with this manager.
	 *
	 * @return A weak pointer to the GameModeBase instance, if available. If no GameMode is associated, it returns nullptr.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Manager")
	AGameModeBase* GetGameMode() const;
	
	template<typename T>
	T* GetGameMode() const
	{
		static_assert(TIsDerivedFrom<T, AGameModeBase>::IsDerived);
		return Cast<T>(GetGameMode());
	}


	/**
	 * Starts the gameplay manager logic. This is a virtual method intended to be overridden by derived classes
	 * to implement specific initialization or start-up functionality for the manager.
	 *
	 * By default, this method is invoked during the GameMode's `BeginPlay` lifecycle, provided the manager passes
	 * the checks defined in the `CanStart()` method. For example, ensuring that only one instance of the manager
	 * exists, and the world context is valid and in a game state. This method is a central entry point for
	 * manager functionality initialization.
	 *
	 * In derived classes, ensure to call the superclass's `Start()` implementation if additional setup
	 * behavior is desired, or override entirely for specialized functionality. Blueprint implementation
	 * hooks should also be considered, such as `BP_Start` for Blueprint necessary logic on start.
	 *
	 * This method ensures that the manager is correctly initialized and sets the internal state indicating its readiness.
	 */
	virtual void Start();
	/**
	 * Determines whether this instance of the gameplay manager can start.
	 *
	 * A manager can start only if there is exactly one instance of its class in the world,
	 * the world context is valid, and the world is a game world.
	 *
	 * @return True if the manager can start, otherwise false.
	 */
	virtual bool CanStart() const;

	/**
	 * Blueprint event triggered when the gameplay manager is initialized and its logic has started.
	 *
	 * This event is called after the manager successfully passes the checks defined in `CanStart()` and
	 * the `Start()` function is executed. Derived classes or Blueprint implementations can use this
	 * event to execute additional initialization or gameplay-specific logic.
	 *
	 * To override this behavior in Blueprint, implement the event logic in the Blueprint class
	 * inheriting from `ALimenGameplayManager`.
	 *
	 * This function is intended for Blueprint-specific logic and should not be used for critical
	 * C++-level logic that requires guarantees of execution.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Manager", meta=(DisplayName="On Manager Initialized"))
	void BP_Start();

private:
	TWeakObjectPtr<AGameModeBase> GameModeWeakPtr;

	bool bHasInitialized;

	/**
	 * Retrieves a list of gameplay manager instances of a specified class within a given world.
	 *
	 * This method searches for and collects all instances of the specified gameplay manager class
	 * present in the provided world. It is used to identify and return gameplay managers that match
	 * the desired criteria.
	 *
	 * @param World The world context in which to search for gameplay manager instances.
	 * @param ManagerClass The class type of gameplay managers to search for.
	 * @return An array of gameplay manager instances matching the specified class within the given world.
	 */
	static TArray<ALimenGameplayManager*> GetGameplayManagers(const UWorld* World, const TSubclassOf<ALimenGameplayManager>& ManagerClass);

};
