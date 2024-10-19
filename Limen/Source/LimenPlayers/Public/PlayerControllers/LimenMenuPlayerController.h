// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenPlayerController.h"
#include "Widgets/LimenMainMenuWidget.h"
#include "LimenMenuPlayerController.generated.h"

class ALimenMenuHUD;
class ULimenSaveData;
class ULimenGameParameters;
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

	void RequestNewGame(APlayerController* InPlayerController, ULimenGameParameters* Params);
	void RequestLoadGame(APlayerController* InPlayerController, ULimenSaveData* SaveGame);
	void QuitGame(APlayerController* InPlayerController);

protected:
	virtual void UnbindPawnDelegates(APawn* InPawn) override;
	
	virtual void BindWidgetDelegates() override;
	virtual void UnbindWidgetDelegates() override;

	virtual void RequestNotificationPause(const bool bWantsToPause) override;
	
	virtual void LoadingScreenVisibilityChanged(const bool bIsVisible) override;
	virtual bool CreateHudReference() override;	

private:
	UPROPERTY()
	TWeakObjectPtr<ALimenMenuHUD> MenuHud;
	
};
