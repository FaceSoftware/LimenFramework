// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Tickable.h"
#include "Engine/TimerHandle.h"
#include "GameFramework/Actor.h"
#include "Interface/LimenAsyncInitializer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Widgets/LimenWidget.h"
#include "LimenLevelTransitionSubsystem.generated.h"


class IInputProcessor;
class ULimenLoadingScreenParameters;
class ULimenLoadingScreenWidget;

/**
 * 
 */
UCLASS(Config="Limen")
class LIMENLEVELTRANSITIONS_API ULimenLevelTransitionSubsystem : public UGameInstanceSubsystem, public FTickableGameObject
{
	GENERATED_BODY()

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProgress, const int32, Total, const int32, Left);

public:
	FLimenWidgetVisibilityChanged OnLoadingScreenVisibilityChanged; 
	UPROPERTY(BlueprintAssignable)
	FProgress OnPSOBatchingUpdated;

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
	UFUNCTION(BlueprintCallable, Category="Limen|Level Transition Subsystem")
	bool PlayLoadingScreenForCurrentLevel();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Level Transition Subsystem")
	int32 GetPSOsLeft() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Level Transition Subsystem")
	int32 GetTotalPSOs() const;

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
	TMap<TSoftObjectPtr<UWorld>, TSoftObjectPtr<ULimenLoadingScreenParameters>> LoadingScreens;
	TStrongObjectPtr<ULimenLoadingScreenParameters> CurrentLoadingScreenSettings;
	
	TWeakObjectPtr<ULimenLoadingScreenWidget> LoadingScreenWidget;
	TSharedPtr<IInputProcessor> InputPreProcessor;
	
	TWeakObjectPtr<UWorld> LoadedWorld;
	
	bool bIsPreLoadingLevel;
	bool bIsLoadingScreenNotifiedToHide;

	float SecondsShown = 0;
	mutable float PostCompletedSecondsShown = 0;

	bool bDebugShowLoadingScreen = false;

	FTimerHandle RemoveLoadingScreenTimerHandle;

	double TransientMasterVolumeCachedValue;

	uint32 TotalPSOs;
	uint32 PSOsLeft;

	void EnableAudio();
	void DisableAudio();

	UFUNCTION()
	void LoadingScreenVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	void LoadingScreenAnimationFinished(const bool bIsVisible);
};

/**
 * @brief Works as a proxy for UObjects to let the ULimenLevelTransitionSubsystem know that work is being done
 */
UCLASS()
class LIMENLEVELTRANSITIONS_API AInitializerProxyActor : public AActor, public ILimenAsyncInitializer
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static AInitializerProxyActor* CreateInitializerProxyActor(UObject* WorldContextObject);
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject"))
	static void DestroyInitializerProxyActor(UObject* WorldContextObject, AInitializerProxyActor* InitProxyActor);
	
	AInitializerProxyActor();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual bool IsWorking_Implementation() const override;

private:
	bool bIsWorking;
};
