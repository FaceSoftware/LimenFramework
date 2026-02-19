// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/LocalPlayerSubsystem.h"
#include "LimenInputBindingsLocalPlayerSubsystem.generated.h"

struct FEnhancedActionKeyMapping;
/**
 * 
 */
UCLASS()
class LIMENKEYBINDSETTINGS_API ULimenInputBindingsLocalPlayerSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()
	
public:
	ULimenInputBindingsLocalPlayerSubsystem();
	void AddPawnMappingContext();
	void RemovePawnMappingContext();

protected:
	int32 PawnMappingContextPriority;
	int32 PlayerControllerMappingContextPriority;

	virtual void PlayerControllerChanged(APlayerController* NewPlayerController) override;
	
private:
	TWeakObjectPtr<APlayerController> OldPlayerController;
	TWeakObjectPtr<APlayerController> CurrentPlayerController;
	TWeakObjectPtr<APawn> CurrentPawn;
	FDelegateHandle NewPawnDelegateHandle;
	
	void UpdateControllerBindings(APlayerController* PC);
	void InputBindUpdated(const FEnhancedActionKeyMapping& ActionKeyMapping);

	UFUNCTION()
	void NewPawnNotify(APawn* NewPawn);
	UFUNCTION()
	void PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
};
