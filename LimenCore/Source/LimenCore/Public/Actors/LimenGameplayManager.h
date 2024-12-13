// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EngineUtils.h"
#include "Interfaces/LimenCoreObjectInterface.h"
#include "LimenGameplayManager.generated.h"


class AGameModeBase;

/**
 * A gameplay manager is an actor that behaves as a GameMode but for specific things.
 * This approach enforces code separation instead of having everything in the GameMode class, like a subsystem.
 * One example of this is setting the player's spawn position, giving the player's starting items, etc.
 * LimenGameModeBase contains a list where you can add managers and they will automatically be spawned
 * during the GameMode's BeginPlay function.
 * Please note that only one manager of a certain class can be instanced.
 */
UCLASS(Abstract, Blueprintable)
class LIMENCORE_API ALimenGameplayManager : public AInfo, public ILimenCoreObjectInterface
{
	GENERATED_BODY()

public:
	/**
	 * @brief Gets a manager of a specific class.
	 * @param WorldContextObject The UObject to get the world from (ignore in blueprints, "GetWorld()" or "this", if UObject, in c++).
	 * @param ManagerClass The class of the manager to get.
	 * @return The manager, if it exits, can be nullptr/Invalid.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Manager", meta=(WorldContext="WorldContextObject", DeterminesOutputType="ManagerClass"))
	static ALimenGameplayManager* GetGameplayManager(UObject* WorldContextObject, TSubclassOf<ALimenGameplayManager> ManagerClass);

	template<typename ManagerClass>
	static ManagerClass* GetGameplayManager(UObject* WorldContextObject)
	{
		static_assert(std::is_base_of_v<ALimenGameplayManager, ManagerClass>);
		return Cast<ManagerClass>(GetGameplayManager(WorldContextObject, ManagerClass::StaticClass()));
	}

	template <typename ManagerClass>
	static ManagerClass* GetGameplayManager(UWorld* World)
	{
		for (TActorIterator<ALimenGameplayManager> It(World); It; ++It)
		{
			if (auto* Manager = Cast<ManagerClass>(*It); Manager != nullptr)
			{
				return Manager;
			}
		}

		return nullptr;
	}

	template<typename ManagerClass>
	static void GetGameplayManagers(UWorld* World, TArray<ManagerClass*>& OutManagers)
	{
		static_assert(std::is_base_of_v<ALimenGameplayManager, ManagerClass>);
		for (TActorIterator<ManagerClass> It(World); It; ++It)
		{
			OutManagers.Push(*It);
		}
	}
	
	
public:
	FObjectInitializeDelegate OnObjectInitialized;
	
	explicit ALimenGameplayManager(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	/**
	 * @brief Should be called by the game mode class to initialize a reference to it.
	 * To avoid iterating all actors just to get the game mode.
	 * @param InGameMode The game mode class.
	 */
	void BindGameMode(AGameModeBase* InGameMode);

	bool HasInitialized() const;
	

protected:
	/**
	 * @brief Gets the current game mode.
	 * @return The game mode instance.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Manager")
	AGameModeBase* GetGameMode() const;
	
	
	/**
	 * @brief Fired when the manager and it's variables have been setup. (1 tick after begin play)
	 * This is preferred instead of begin play.
	 */
	virtual void Start();
	virtual bool CanStart();

	/**
	 * @brief Called when the manager has initialized.
	 * This is preferred over BeginPlay.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Manager", meta=(DisplayName="On Manager Initialized"))
	void BP_Start();

private:
	TWeakObjectPtr<AGameModeBase> GameModeWeakPtr;

	bool bHasInitialized;
	
	virtual void Initialize() override final;

	/**
	 * @brief Gets all manager of a specific class.
	 * @param World The world to check for managers.
	 * @param ManagerClass The class of the manager to get.
	 * @return The manager, if it exits, can be nullptr/Invalid.
	 * @warning Internal use only. Used to stop multiple multiple instances of a manager spawning.
	 */
	static TArray<ALimenGameplayManager*> GetGameplayManagers(const UWorld* World, const TSubclassOf<ALimenGameplayManager>& ManagerClass);

};
