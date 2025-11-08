// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenWidget.h"
#include "LimenBaseHudWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENWIDGETS_API ULimenBaseHudWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	explicit ULimenBaseHudWidget(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category="Limen|Menus|Player Controller")
	virtual void BindPlayerController(APlayerController* InPlayerController);

protected:
	UFUNCTION(BlueprintNativeEvent, Category="Limen|Menus|Player Controller")
	void OnPlayerControllerBound(APlayerController* InPlayerController);
	virtual void OnPlayerControllerBound_Implementation(APlayerController* InPlayerController) {}

	UFUNCTION(BlueprintNativeEvent, Category="Limen|Events")
	void PawnBoundUpdated(APawn* NewPawn);
	virtual void PawnBoundUpdated_Implementation(APawn* NewPawn) {}

private:
	TWeakObjectPtr<APlayerController> BoundPlayerController;
	TWeakObjectPtr<APawn> BoundPawn;

	UFUNCTION()
	void PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
};
