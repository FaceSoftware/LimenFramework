// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenPlayerController.h"
#include "Widgets/LimenMainMenuWidget.h"
#include "LimenMenuPlayerController.generated.h"

class ALimenMenuHUD;
class ULimenSaveData;
/**
 * 
 */
UCLASS()
class LIMENPLAYERS_API ALimenMenuPlayerController : public ALimenPlayerControllerBase
{
	GENERATED_BODY()
	
public:
	FNewGameRequest OnNewGameRequested;
	FLoadGameRequest OnLoadGameRequested;
	
	ALimenMenuPlayerController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;

protected:
	virtual void UnbindPawnDelegates(APawn* InPawn) override;
	
	virtual void BindWidgetDelegates() override;
	virtual void UnbindWidgetDelegates() override;
	
	virtual void LoadingScreenVisibilityChanged(const bool bIsVisible) override;
	virtual bool CreateHudReference() override;	

private:
	UPROPERTY()
	TWeakObjectPtr<ALimenMenuHUD> MenuHud;

	void RequestNewGame(APlayerController* InPlayerController);
	void RequestLoadGame(APlayerController* InPlayerController, const ULimenSaveData* SaveGame);
	void QuitGame(APlayerController* InPlayerController);
};
