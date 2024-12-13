// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenGameStateBase.h"
#include "LimenGameState.generated.h"

class ALimenPlayerState;
class ALimenGameMode;
/**
 * 
 */
UCLASS()
class LIMENGAMEMODES_API ALimenGameState : public ALimenGameStateBase
{
	GENERATED_BODY()


public:
	explicit ALimenGameState(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	
	void AddPlayerToPlayerList(ALimenPlayerState* InPlayerState);
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	virtual void OnRep_LimenPlayerList(const TArray<ALimenPlayerState*>& NewLimenPlayerList);

private:
	UPROPERTY(ReplicatedUsing="OnRep_LimenPlayerList")
	TArray<ALimenPlayerState*> LimenPlayerList;
};
