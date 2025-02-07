// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LimenGameModeBase.generated.h"


class ALimenGameplayManager;
class ALimenGameStateBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FPauseDelegate, const bool /* bIsPaused */);

UCLASS(Blueprintable)
class LIMENGAMEFRAMEWORK_API ALimenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	FPauseDelegate OnPauseStateChanged;
	
	ALimenGameModeBase();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;

	ALimenGameStateBase* GetLimenGameState();

	void ResetManagers();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSoftClassPtr<ALimenGameplayManager>> ManagersClassList;

	virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;
	virtual bool ClearPause() override;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<ALimenGameplayManager>> ManagersList;

	UPROPERTY()
	TObjectPtr<ALimenGameStateBase> LimenGameState;
	
	virtual void SpawnManagers();
};
