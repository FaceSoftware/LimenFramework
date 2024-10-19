// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenGameMenuWidget.h"
#include "LimenMenuWrapperWidget.generated.h"

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenMenuWrapperWidget : public ULimenGameMenuWidget
{
	GENERATED_BODY()

public:
	ULimenMenuWrapperWidget();

protected:
	/**
	 * @brief Closes this menu and sets the input to the game.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ReturnToGame() {}
	
	virtual void OnPlayerControllerBound_Implementation(APlayerController* InPlayerController) override;
	virtual void PawnBoundUpdated_Implementation(APawn* NewPawn) override;
	
	UFUNCTION(BlueprintNativeEvent)
	void OnWidgetInitialized();
	void OnWidgetInitialized_Implementation() {}

private:
	uint8 NumChecks;

	void CheckInitialization_Internal();
};
