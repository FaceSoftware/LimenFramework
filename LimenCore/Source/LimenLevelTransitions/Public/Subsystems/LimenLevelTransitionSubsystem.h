// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interface/LimenAsyncInitializer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenLevelTransitionSubsystem.generated.h"


class ULimenLoadingScreenParameters;
class ULimenLoadingScreenWidget;
/**
 * 
 */
UCLASS(Config="Limen")
class LIMENLEVELTRANSITIONS_API ULimenLevelTransitionSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLoadingScreenEvent, const bool, bIsShowing);
	DECLARE_MULTICAST_DELEGATE(FLoadingScreenDelegate);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FShaderCompilingProgress, const double, Progress);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FShaderCompilingDetailedProgress, const int32, RemainingShaders, const int32, TotalShaders);

public:
	UPROPERTY(BlueprintAssignable)	
	FLoadingScreenEvent OnLoadingScreenVisibilityChanged;
	FLoadingScreenDelegate OnLoadingScreenVisible;
	FLoadingScreenDelegate OnLoadingScreenHidden;

	ULimenLevelTransitionSubsystem();
	
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;

	UFUNCTION(BlueprintCallable, Category="Limen|Level Transition Subsystem")
	bool IsLoadingScreenActive() const;

protected:
	void UpdateLoadingScreen(float DeltaTime);
	
	void DisplayLoadingScreen();
	void HideLoadingScreen();

	bool ShouldShowLoadingScreen() const;
	
	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	void HandlePostLoadMap(UWorld* World);

	void BlockInput();
	void UnblockInput();

	void ChangePerformanceSettings(bool bEnablingLoadingScreen) const;

private:
	UPROPERTY()
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<ULimenLoadingScreenParameters>> LoadingScreens;

	UPROPERTY()
	TObjectPtr<ULimenLoadingScreenParameters> CurrentLoadingScreenSettings;

	UPROPERTY()
	TObjectPtr<ULimenLoadingScreenWidget> LoadingScreenWidget;
	TSharedPtr<IInputProcessor> InputPreProcessor;

	UPROPERTY()
	TObjectPtr<UWorld> LoadedWorld;
	
	bool bIsPreLoadingLevel;
	bool bIsLoadingScreenNotifiedToHide;

	float SecondsShown = 0;

	bool bDebugShowLoadingScreen = false;

	FTimerHandle RemoveLoadingScreenTimerHandle;

	double TransientMasterVolumeCachedValue;
	
	UFUNCTION()
	void HideLoadingScreen_Internal();

	UFUNCTION()
	void LoadingScreenAnimationFinished(const bool bIsVisibleAnimation);

	void EnableAudio();
	void DisableAudio();
};

/**
 * @brief Works as a proxy for UObjects to let the ULimenLevelTransitionSubsystem know that work is being done
 */
UCLASS()
class LIMENLEVELTRANSITIONS_API AInitializerProxyActor : public AActor, public ILimenAsyncInitializer
{
	GENERATED_BODY()

public:
	virtual bool IsWorking_Implementation() const override;
};
