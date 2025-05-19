// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "GameFramework/GameModeBase.h"
#include "LimenGameModeBase.generated.h"


class ALimenGameplayManager;
class ALimenGameStateBase;

UCLASS(Blueprintable)
class LIMENGAMEFRAMEWORK_API ALimenGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	explicit ALimenGameModeBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual void InitSeamlessTravelPlayer(AController* NewController) override;

	ALimenGameStateBase* GetLimenGameState();
	template<typename T>
	T* GetManager() const
	{
		auto* M = ManagersList.FindByPredicate(
			[] (const ALimenGameplayManager* Test)
			{
				return Test->IsA<T>();
			});

		if (M == nullptr)
		{
			return nullptr;
		}

		return CastChecked<T>(*M);
	}

	void ResetManagers();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Classes")
	TArray<TSoftClassPtr<ALimenGameplayManager>> ManagersClassList;
	
private:
	UPROPERTY()
	TArray<TObjectPtr<ALimenGameplayManager>> ManagersList;

	UPROPERTY()
	TObjectPtr<ALimenGameStateBase> LimenGameState;
	
	virtual void SpawnManagers();
};
