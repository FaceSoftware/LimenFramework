// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "GameFramework/GameModeBase.h"
#include "LimenGameModeBase.generated.h"


class ALimenPlayerControllerBase;
class ALimenGameplayManager;
class ALimenGameStateBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGamePauseDelegate, bool, bIsPaused);

UCLASS(Blueprintable)
class LIMENGAMEFRAMEWORK_API ALimenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FGamePauseDelegate OnGamePauseStateChanged;

	explicit ALimenGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;
	virtual bool ClearPause() override;
	virtual void Tick(float DeltaSeconds) override;
	

	ALimenGameStateBase* GetLimenGameState();
	template<typename T>
	T* GetManager() const
	{
		static_assert(TIsDerivedFrom<T, ALimenGameplayManager>::IsDerived);
		for (auto& [Class, ManagerWeakPtr] : ManagersList)
		{
			if (T* ManagerPtr = Cast<T>(ManagerWeakPtr.Get()); UNLIKELY(ManagerPtr))
			{
				return ManagerPtr;
			}
		}
		return nullptr;
	}
	template<typename T>
	TArray<T*> GetManagers() const
	{
		static_assert(TIsDerivedFrom<T, ALimenGameplayManager>::IsDerived);
		
		TArray<T*> FoundManagers;
		for (auto& [Class, ManagerWeakPtr] : ManagersList)
		{
			if constexpr (std::is_same_v<T, ALimenGameplayManager>)
			{
				FoundManagers.Push(ManagerWeakPtr.Get());
			}
			else if (T* ManagerPtr = Cast<T>(ManagerWeakPtr.Get()))
			{
				FoundManagers.Push(ManagerPtr);
			}
		}
		return FoundManagers;
	}
	void ResetManagers();
	bool IsInitialLevelStreamingComplete() const;
	
	template<typename ManagerClass>
	void RegisterActorWithManager(AActor* Actor, const FGameplayTag& Id)
	{
		static_assert(TIsDerivedFrom<ManagerClass, ALimenGameplayManager>::IsDerived);
		for (ALimenGameplayManager* ManagerPtr : GetManagers<ManagerClass>())
		{
			ManagerPtr->NotifyActorRegistry(Id, Actor);
		}
	}
	void RegisterActorWithManager(AActor* Actor, const FGameplayTag& Id);
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSoftClassPtr<ALimenGameplayManager>> ManagersClassList;
	
	virtual void BeginPlay() override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
	
	virtual void InitialLevelStreamingComplete();
	virtual void ClientReady(ALimenPlayerControllerBase* PlayerController);
	virtual void TickGameState(AGameStateBase* InGameState, float DeltaSeconds);
	virtual void TickPlayerState(APlayerState* InPlayerState, float DeltaSeconds);

private:
	TMap<TSubclassOf<ALimenGameplayManager>, TWeakObjectPtr<ALimenGameplayManager>> ManagersList;
	TWeakObjectPtr<ALimenGameStateBase> LimenGameState;
	int32 InitialStreamingLevels;
	TArray<TWeakObjectPtr<ULevelStreaming>> InitialStreamingLevelsList;
	
	virtual void SetupManuallyPlacedManagers();
	virtual void SpawnManagers();
	UFUNCTION()
	void InitialStreamingLevelShown();
};
