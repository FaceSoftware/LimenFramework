// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "GameFramework/GameModeBase.h"
#include "LimenGameModeBase.generated.h"


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
		auto* M = ManagersList.FindByPredicate(
			[] (const TWeakObjectPtr<ALimenGameplayManager>& Test)
			{
				return Test->IsA<T>();
			});

		if (M == nullptr) { return nullptr; }
		return CastChecked<T>(*M);
	}

	void ResetManagers();
	bool IsInitialLevelStreamingComplete() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSoftClassPtr<ALimenGameplayManager>> ManagersClassList;
	
	virtual void BeginPlay() override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;
	virtual void InitialLevelStreamingComplete();
	virtual void TickGameState(AGameStateBase* InGameState, float DeltaSeconds);
	virtual void TickPlayerState(APlayerState* InPlayerState, float DeltaSeconds);

private:
	TArray<TWeakObjectPtr<ALimenGameplayManager>> ManagersList;
	TWeakObjectPtr<ALimenGameStateBase> LimenGameState;
	int32 InitialStreamingLevels;
	TArray<TWeakObjectPtr<ULevelStreaming>> InitialStreamingLevelsList;
	
	virtual void SpawnManagers();
	UFUNCTION()
	void InitialStreamingLevelShown();
};
