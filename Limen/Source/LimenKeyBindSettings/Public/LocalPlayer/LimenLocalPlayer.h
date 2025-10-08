// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputSubsystemInterface.h"
#include "Engine/LocalPlayer.h"
#include "LimenLocalPlayer.generated.h"

struct FModifyContextOptions;
class UInputMappingContext;
struct FEnhancedActionKeyMapping;


/**
 * 
 */
UCLASS()
class LIMENKEYBINDSETTINGS_API ULimenLocalPlayer : public ULocalPlayer
{
	GENERATED_BODY()

public:
	ULimenLocalPlayer();
	void AddPawnMappingContext();
	void RemovePawnMappingContext();

protected:
	int32 PawnMappingContextPriority;
	int32 PlayerControllerMappingContextPriority;

	virtual void ReceivedPlayerController(APlayerController* NewController) override;
	
private:
	TWeakObjectPtr<APlayerController> OldPlayerController;
	TWeakObjectPtr<APlayerController> CurrentPlayerController;
	TWeakObjectPtr<APawn> CurrentPawn;
	FDelegateHandle NewPawnDelegateHandle;
	
	void UpdateControllerBindings(APlayerController* PC);
	void InputBindUpdated(const FEnhancedActionKeyMapping& ActionKeyMapping);

	UFUNCTION()
	void PossessedPawnChanged(APawn* NewPawn);
};
