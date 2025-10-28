// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenPlayerCharacter.h"
#include "LimenAICharacter.generated.h"

class ULimenProximityInteractionComponent;
class ALimenAIControllerBase;

UCLASS(Blueprintable, BlueprintType)
class LIMENPLAYERS_API ALimenAICharacter : public ALimenGameplayCharacter
{
	GENERATED_BODY()

public:
	explicit ALimenAICharacter(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable)
	void Freeze(const bool bFreezePose = true);
	UFUNCTION(BlueprintCallable)
	void UnFreeze();
	UFUNCTION(BlueprintCallable)
	bool IsFrozen() const;
	
	UFUNCTION(BlueprintCallable)
	bool IsBeingSeenByPlayer(const APlayerController* InPlayerController) const;

	UFUNCTION(BlueprintCallable)
	void ResumeBrainLogic(const FString& Reason);
	UFUNCTION(BlueprintCallable)
	void PauseBrainLogic(const FString& Reason);

	virtual void Interact() override;
	virtual ULimenInteractionComponent* GetInteractionComponent() const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULimenProximityInteractionComponent> InteractionComponent;

	virtual void NotifyControllerChanged() override;
	
private:
	EMovementMode MovementModeBeforeFreeze;
	bool bIsFrozen;
	FRotator FrozenRotation;
	TWeakObjectPtr<ALimenAIControllerBase> LimenAIController;
};
