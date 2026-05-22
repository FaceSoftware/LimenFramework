// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenMenuWidget.h"
#include "LimenGameMenuWidget.generated.h"


class ALimenBaseCharacter;

UCLASS(Abstract, Blueprintable)
class LIMENWIDGETS_API ULimenGameMenuWidget : public ULimenMenuWidget
{
	GENERATED_BODY()

public:
	virtual void BindPlayerController(APlayerController* InPlayerController) override;

protected:	
	UFUNCTION(BlueprintCallable, Category="Limen|Game Menu")
	APawn* GetBoundPawn() const;

	UFUNCTION(BlueprintNativeEvent, Category="Limen|Events")
	void PawnBoundUpdated(APawn* NewPawn);
	virtual void PawnBoundUpdated_Implementation(APawn* NewPawn) {}

private:
	TWeakObjectPtr<APawn> BoundPawn;
	TWeakObjectPtr<APlayerController> BoundPlayerController;
	
	UFUNCTION()
	void PossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
};
