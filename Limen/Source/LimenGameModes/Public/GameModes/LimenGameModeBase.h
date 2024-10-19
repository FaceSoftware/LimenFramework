// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "LimenGameModeBase.generated.h"

class ALimenGameState;
class ALimenGameplayManager;
class ALimenBasePlayerController;
class ALimenBaseCharacter;

UCLASS(Abstract, NotBlueprintable)
class LIMENGAMEMODES_API ALimenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	ALimenGameModeBase();
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;

	ALimenGameState* GetLimenGameState();
	
protected:

private:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSoftClassPtr<ALimenGameplayManager>> ManagersClassList;
	UPROPERTY()
	TArray<TObjectPtr<ALimenGameplayManager>> ManagersList;

	UPROPERTY()
	TObjectPtr<ALimenGameState> LimenGameState;
	
	virtual void SpawnManagers();
};
