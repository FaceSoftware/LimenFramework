// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "Interface/LimenAsyncInitializer.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenProceduralMapBuilder.generated.h"


class UTileMapDataAsset;
class ALimenProceduralMapManager;
class UProceduralMapDataAsset;
class ULimenGlobalRandomStreamSubsystem;
class FMapBuilderThread;

UENUM()
enum class EMapBuildStage : uint8
{
	Calculations,
	Spawning,
	Cleanup
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMapUpdate, const int64, MapIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FDetailedBuildProcessDelegate, const float, GlobalProgressPercentage,
											  const EMapBuildStage, CurrentStage, const float, CurrentStep, const float,
											  LastStep);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCustomBuildProgress, const FString&, ProgressMessage);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBuildProcessDelegate);
DECLARE_DELEGATE(FBuildStep);

UCLASS(Abstract, NotBlueprintable)
class PROCEDURALMAPS_API ALimenProceduralMapBuilder : public ALimenGameplayManager, public ILimenAsyncInitializer
{
	GENERATED_BODY()
	
public:
	friend FMapBuilderThread;

	UPROPERTY(BlueprintAssignable)
	FMapUpdate OnMapLoaded;

	UPROPERTY(BlueprintAssignable)
	FMapUpdate OnMapUnloaded;

	UPROPERTY(BlueprintAssignable)
	FMapUpdate OnMapStartedBuilding;
	
	UPROPERTY(BlueprintAssignable, Category="Limen|Procedural Map Builder")
	FDetailedBuildProcessDelegate OnBuildProcessUpdated;
	UPROPERTY(BlueprintAssignable, Category="Limen|Procedural Map Builder")
	FBuildProcessDelegate OnBuildProcessStarted;
	UPROPERTY(BlueprintAssignable, Category="Limen|Procedural Map Builder")
	FBuildProcessDelegate OnBuildProcessFinished;

	ALimenProceduralMapBuilder(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Map Builder")
	virtual void StartBuildProcess();

	void SetSeed(const uint64 NewSeed);
	uint64 GetSeed() const;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Map Builder")
	bool IsMapBuilt() const { return bIsMapBuilt; }

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Misc")
	virtual void UnloadMap();
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Misc")
	virtual void DestroyPreviousMap();
	virtual bool SetCurrentMapDataAssetByName(const FName MapName);
	virtual bool SetCurrentMapDataAssetByRef(UProceduralMapDataAsset* MapDataAsset);
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	virtual bool LoadNextMap();
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	virtual bool LoadMapWithIndex(const int Index);
	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Misc")
	virtual int GetCurrentMapIndex() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps|Tiles")
	UProceduralMapDataAsset* GetCurrentMapData() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Maps")
	ALimenProceduralMapManager* GetCurrentMapManager() const;

	
	virtual bool IsWorking_Implementation() const override;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Tile Maps")
	TMap<FName, UProceduralMapDataAsset*> Maps;
	UPROPERTY()
	TObjectPtr<UProceduralMapDataAsset> CurrentMap;
	UPROPERTY()
	TWeakObjectPtr<ALimenProceduralMapManager> CurrentMapManager;
	uint32 CurrentMapIndex;
	
	TArray<FBuildStep> CalculationSteps;
	TArray<FBuildStep> GameThreadSteps;
	TArray<FBuildStep> CleanupSteps;

	TSharedRef<FRandomStream> GetRandomStream();
	
	UFUNCTION(BlueprintImplementableEvent)
	void PreBuildStart();

	/**
	 * @brief Adds a function to the calculation stage.
	 * @param FunctionName The name of the function to add.
	 * @warning Runs in a separate thread (not in the game thread).
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Add Calculation Step", Category="Limen|Procedural Map Builder")
	void BP_AddCalculationStep(const FName FunctionName);
	/**
	 * @brief Adds a function to the game thread stage, useful for changes in the world (like spawning actors).
	 * @param FunctionName The name of the function to add.
	 * @warning Runs in the Game Thread.
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Add Game Thread Step", Category="Limen|Procedural Map Builder")
	void BP_AddGameThreadStep(const FName FunctionName);
	/**
	 * @brief Adds a function to the cleanup stage, usually where any resources used in the calculation stage are cleaned up. 
	 * @param FunctionName The name of the function to add.
	 * @warning Runs in a separate thread (not in the game thread).
	 */
	UFUNCTION(BlueprintCallable, DisplayName="Add Cleanup Step", Category="Limen|Procedural Map Builder")
	void BP_AddCleanupStep(const FName FunctionName);

	UFUNCTION()
	virtual void OnBuildingMapStarted();
	UFUNCTION()
	virtual void OnBuildingMapFinished();

	TPromise<bool>& GetGameThreadStageCompletePromise();

	bool ShouldStopMapBuilderThread() const;

	UFUNCTION(BlueprintNativeEvent, Category="Limen|Procedural Maps")
	void FinalLevelCompleted();
	virtual void FinalLevelCompleted_Implementation();

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Generation Parameters")
	uint64 Seed;

	TUniquePtr<FMapBuilderThread> MapInitializerRunnable;
	TUniquePtr<FRunnableThread> MapInitializerThread;

	uint8 bIsMapBuilt : 1;
	bool bIsWorking;
	uint32 NumMapsBuilt;
	
	bool bShouldStopMapBuilderThread;

	void SetupMapCompletionListener();
};

/**
 * @brief Helper macro to add a function as a calculation step.
 * @param Object The owner of the function.
 * @param FunctionRef A reference to the function.
 */
#define ADD_CALCULATION_STEP(Object, FunctionRef) \
{ \
FBuildStep Step; Step.BindUObject(Object, FunctionRef); CalculationSteps.Push(Step); \
} \

/**
 * @brief Helper macro to add a function as a game thread step.
 * @param Object The owner of the function.
 * @param FunctionRef A reference to the function.
 */
#define ADD_GAMETHREAD_STEP(Object, FunctionRef) \
{ \
FBuildStep Step; Step.BindUObject(Object, FunctionRef); GameThreadSteps.Push(Step); \
} \

/**
 * @brief Helper macro to add a function as a cleanup step.
 * @param Object The owner of the function.
 * @param FunctionRef A reference to the function.
 */
#define ADD_CLEANUP_STEP(Object, FunctionRef) \
{ \
FBuildStep Step; Step.BindUObject(Object, FunctionRef); CleanupSteps.Push(Step); \
} \

class PROCEDURALMAPS_API FMapBuilderThread final : public FRunnable
{
public:
	FMapBuilderThread(ALimenProceduralMapBuilder* InInstance, const TArray<FBuildStep>& InCalculationSteps, const TArray<FBuildStep>& InGameThreadSteps, const TArray<FBuildStep>& InCleanupSteps);

	virtual ~FMapBuilderThread() override;

	virtual bool Init() override;
	
	virtual uint32 Run() override;
	
	virtual void Stop() override;
	
	virtual void Exit() override;

	TPromise<bool>& GetGameThreadStageCompletePromise();
	bool HasGameThreadStageCompleteResponse() const;

	void RequestEarlyStop();

private:
	uint32 GlobalSteps;
	float GlobalProgress;
	float SingleStepProgress;
	
	TWeakObjectPtr<ALimenProceduralMapBuilder> InstanceWeakPtr;
	TArray<FBuildStep> CalculationSteps;
	TArray<FBuildStep> GameThreadSteps;
	TArray<FBuildStep> CleanupSteps;

	FThreadSafeBool bShouldStop;

	bool bGotGameThreadStageCompleteResponse;
	TPromise<bool> GameThreadStageComplete;
	uint32 Calculate();
	uint32 Spawn();
	uint32 Cleanup();
};
