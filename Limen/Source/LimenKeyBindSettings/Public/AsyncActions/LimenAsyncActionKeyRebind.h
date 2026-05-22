// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedActionKeyMapping.h"
#include "Actors/LimenGameplayManager.h"
#include "Engine/CancellableAsyncAction.h"
#include "Engine/TimerHandle.h"
#include "UMG/LimenGenericModalWidget.h"
#include "LimenAsyncActionKeyRebind.generated.h"


class ALimenInputListener;
enum class ELimenInputMode : uint8;
class ULimenGenericModalWidget;
class ALimenPlayerControllerBase;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnKeyPressed, FEnhancedActionKeyMapping, PressedKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTimeoutDelegate);

/**
 * 
 */
UCLASS()
class LIMENKEYBINDSETTINGS_API ULimenAsyncActionKeyRebind : public UBlueprintAsyncActionBase, public FTickableGameObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnKeyPressed OnKeyPressed;
	UPROPERTY(BlueprintAssignable)
	FTimeoutDelegate OnTimeout;
	
	ULimenAsyncActionKeyRebind();

	virtual void BeginDestroy() override;
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Key binds", meta=(WorldContext="InWorldContext"))
	static ULimenAsyncActionKeyRebind* RebindKey(UObject* InWorldContext, ALimenPlayerControllerBase* PlayerController,
		FEnhancedActionKeyMapping KeyMapping, const float InTimeoutTime);

#pragma region FTickableGameObject

	virtual bool IsTickableWhenPaused() const override;
	virtual bool IsTickableInEditor() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	virtual void Tick(const float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;

#pragma endregion

private:
	static const FString Title;
	static const FString Content;
	static const FString TimeRemainingKeyword;
	
	TWeakObjectPtr<UObject> WorldContext;
	TWeakObjectPtr<ALimenPlayerControllerBase> PlayerController;
	FEnhancedActionKeyMapping KeyMapping;
	float TimeoutTime;
	
	FModalParams ModalParams;

	UPROPERTY()
	TObjectPtr<ULimenGenericModalWidget> DisplayModal;
	UPROPERTY()
	TWeakObjectPtr<ALimenInputListener> InputListener;

	ELimenInputMode PreviousInputMode;

	bool bShouldTick;
	float TotalTickTime;
	float SecondCounter;

	void ResetStates(const bool bIsBeingDestroyed) const;
	void TimedOut();
	void UpdateModal();
};

UCLASS(NotBlueprintType, NotBlueprintable)
class ALimenInputListener : public ALimenGameplayManager
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FKeyPressedDelegate, FKey /* PressedKey */);
	FKeyPressedDelegate OnKeyPressed;
	
	virtual void EnableInput(APlayerController* PlayerController) override;

private:
	void KeyPressed(FKey PressedKey);
	
};
