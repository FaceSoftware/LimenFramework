// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenModularSavesSubsystem.generated.h"

class ULimenModularSaveData;
class ULimenSaveSubsystem;
class ULimenGameSaveData;
struct FActorSaveData;


DECLARE_MULTICAST_DELEGATE(FGameSaveDelegate);

/**
 * 
 */
UCLASS(Abstract)
class LIMENSAVESYSTEM_API ULimenModularSavesSubsystem : public UGameInstanceSubsystem, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:
	FGameSaveDelegate OnGameDataLoaded;
	
	ULimenModularSavesSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves", meta=(WorldContext="Caller"))
	void SaveCurrentGame(UObject* Caller);
	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves", meta=(WorldContext="Caller"))
	void LoadCurrentGame(UObject* Caller);

	void SaveCurrentGame(UWorld* InWorld);
	void LoadCurrentGame(UWorld* InWorld);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Game Saves")
	virtual void ScheduleLoadOnMapChange();

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
	void DeleteSave();

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void DataSaved()override;
	virtual void DataLoaded() override;

protected:
	FName SaveDataName;
	
private:
	
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<ULimenModularSaveData> CurrentGameSaveData;
	UPROPERTY()
	TObjectPtr<ULimenSaveSubsystem> SaveSubsystem;

	bool bHasLoadedGameData;
	bool bShouldLoadGameOnMapChange;

	void SaveToCurrentSaveData();
	void LoadToCurrentSaveData();

	void InitializeHandlersForSaving();
	void InitializeHandlersForLoading();
	
	void OnWorldActorsInitialized(const FActorsInitializedParams& InitializationParams);
};
