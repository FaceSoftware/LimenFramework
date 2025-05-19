// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/LocalPlayer.h"
#include "LimenLocalPlayer.generated.h"

struct FEnhancedActionKeyMapping;


/**
 * 
 */
UCLASS()
class LIMENKEYBINDSETTINGS_API ULimenLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

protected:
	virtual void ReceivedPlayerController(APlayerController* NewController) override;
	
private:
	TWeakObjectPtr<APlayerController> OldPlayerController;
	TWeakObjectPtr<APlayerController> CurrentPlayerController;
	FDelegateHandle NewPawnDelegateHandle;
	
	void UpdateControllerBindings(APlayerController* PC);
	void InputBindUpdated(const FEnhancedActionKeyMapping& ActionKeyMapping);
	void NewPawnSet(APawn* NewPawn);
	UFUNCTION()
	void PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
};
