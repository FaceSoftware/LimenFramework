// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LimenGameModeBase.generated.h"


class ALimenGameplayManager;
class ALimenGameStateBase;

UCLASS(Abstract, NotBlueprintable)
class LIMENGAMEFRAMEWORK_API ALimenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALimenGameModeBase();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;

	ALimenGameStateBase* GetLimenGameState();

	void ResetManagers();
	
protected:

private:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSoftClassPtr<ALimenGameplayManager>> ManagersClassList;
	UPROPERTY()
	TArray<TObjectPtr<ALimenGameplayManager>> ManagersList;

	UPROPERTY()
	TObjectPtr<ALimenGameStateBase> LimenGameState;
	
	virtual void SpawnManagers();
};
