// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenGameSaveSubsystem.generated.h"

class ULimenSaveSubsystem;
class ULimenGameSaveData;
struct FActorSaveData;


DECLARE_MULTICAST_DELEGATE(FGameSaveDelegate);

/**
 * 
 */
UCLASS()
class LIMENGAMESAVES_API ULimenGameSaveSubsystem : public UGameInstanceSubsystem, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:
	FGameSaveDelegate OnGameDataLoaded;
	
	ULimenGameSaveSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves", meta=(WorldContext="Caller"))
	void SaveCurrentGame(UObject* Caller);
	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves", meta=(WorldContext="Caller"))
	void LoadCurrentGame(UObject* Caller);

	void SaveCurrentGame(UWorld* InWorld);
	void LoadCurrentGame(UWorld* InWorld);

	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves", meta=(WorldContext="Caller"))
	virtual void ScheduleGameLoadOnMapChange();

	/**
	 * @brief Checks if the subsystem has loaded data into the current world.
	 * @return True if the data was loaded, false if not.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves")
	bool HasLoadedGameData() const;
	/**
	 * @brief Checks if a save file exists.
	 * @return True if it exists, false if it does not.
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves")
	bool DoesSavedDataExist() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves")
	void DeleteGameSave();

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void DataSaved()override;
	virtual void DataLoaded() override;

protected:
	
private:
	inline static const FName SaveDataName = TEXT("GameSave");
	
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<ULimenGameSaveData> CurrentGameSaveData;
	UPROPERTY()
	TObjectPtr<ULimenSaveSubsystem> SaveSubsystem;

	bool bHasLoadedGameData;
	bool b;
	bool bShouldLoadGameOnMapChange;

	void SaveToCurrentSaveData();
	void LoadToCurrentSaveData();

	void InitializeHandlersForSaving();
	void InitializeHandlersForLoading();

	
	void OnWorldActorsInitialized(const FActorsInitializedParams& InitializationParams);
};
