// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenGameSaveSubsystem.generated.h"


enum class ESaveState : uint8;
class ULimenGenericModalWidget;
struct FActorsInitializedParams;
class ULimenSaveSubsystem;
class ULimenGameSaveData;
struct FActorSaveData;

DECLARE_MULTICAST_DELEGATE(FGameSaveDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGameSaveState, ESaveState, SaveState);

/**
 * @brief GameInstance subsystem that manages saving and loading.
 *
 * - Serializes ULimenSavesHandler instances into ULimenGameSaveData.
 * - Owns the active ULimenGameSaveData and provides async save/load.
 * - Exposes delegates for save/load state changes.
 *
 * Notes:
 * - ILimenSaveObjectInterface methods are final: subsystem is always save/load eligible.
 * - Handlers are created as temporary objects and destroyed after serialization.
 * - Uses developer settings to resolve save handlers and modal error handling.
 */

UCLASS()
class LIMENGAMESAVES_API ULimenGameSaveSubsystem : public UGameInstanceSubsystem, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:	
	FGameSaveDelegate OnGameDataLoaded;
	UPROPERTY(BlueprintAssignable)
	FGameSaveState OnGameSaveStateChanged;
	
	ULimenGameSaveSubsystem();
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	
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
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;

	UFUNCTION(BlueprintCallable)
	const ULimenGameSaveData* GetCurrentGameSaveData() const;
	UFUNCTION(BlueprintCallable)
	bool IsGameLoadScheduled() const;

protected:
	void DataSaved(const FString& SaveName, const int32 UserIndex, const bool bSuccess);
	
private:
	static const FString SaveDataName;
	static const FString GameSaveDataFolder;
	
	UPROPERTY()
	TObjectPtr<UWorld> World;
	UPROPERTY()
	TObjectPtr<ULimenGameSaveData> CurrentGameSaveData;
	UPROPERTY()
	TObjectPtr<ULimenSaveSubsystem> SaveSubsystem;

	bool bHasLoadedGameData;
	bool bShouldLoadGameOnMapChange;

	ESaveState CurrentSaveState;

	void SaveToCurrentSaveData();
	void LoadToCurrentSaveData();

	bool InitializeHandlersForSaving();
	bool InitializeHandlersForLoading();

	void OnWorldActorsInitialized(const FActorsInitializedParams& InitializationParams);

	UFUNCTION()
	void FailedToLoadDataModalDismissed(ULimenGenericModalWidget* ModalWidget, bool bAccepted);
	UFUNCTION()
	void FailedToSaveDataModalDismissed(ULimenGenericModalWidget* ModalWidget, bool bAccepted);

	void DisplaySaveErrorModal();
};
