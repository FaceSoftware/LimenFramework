// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "LimenGameStateBase.generated.h"


class ALimenPlayerStateBase;

/**
 * 
 */
UCLASS()
class LIMENGAMEFRAMEWORK_API ALimenGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	void AddPlayerToPlayerList(ALimenPlayerStateBase* InPlayerState);
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	virtual void OnRep_LimenPlayerList(const TArray<ALimenPlayerStateBase*>& NewLimenPlayerList);

private:
	UPROPERTY(ReplicatedUsing="OnRep_LimenPlayerList")
	TArray<ALimenPlayerStateBase*> LimenPlayerList;
};
