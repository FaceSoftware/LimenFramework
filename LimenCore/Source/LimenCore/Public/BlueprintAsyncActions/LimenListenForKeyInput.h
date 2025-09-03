// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "LimenListenForKeyInput.generated.h"


struct FKey;
enum class EKeyEvent : uint8;

DECLARE_DYNAMIC_DELEGATE(FListenForKeyInputDelegate);

/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenListenForKeyInput : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContext"))
	static ULimenListenForKeyInput* ListenForKeyInput(UObject* WorldContext, APlayerController* InPlayerController, const FKey& InKey, float InTimeout, FListenForKeyInputDelegate OnKeyPressed, FListenForKeyInputDelegate OnWrongKeyPressed, FListenForKeyInputDelegate OnTimeout);

	virtual void Activate() override;

private:
	TWeakObjectPtr<APlayerController> PlayerController;
	FKey Key;
	FDelegateHandle WorldTickHandle;
	float Timeout;
	bool bUseTimeout;
	FListenForKeyInputDelegate OnKeyPressed;
	FListenForKeyInputDelegate OnWrongKeyPressed;
	FListenForKeyInputDelegate OnTimeout;

	virtual void WorldTick(UWorld* World, ELevelTick LevelTick, float DeltaTime);
};
