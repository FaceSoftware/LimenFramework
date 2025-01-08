// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SaveGames/LimenSaveData.h"
#include "Widgets/LimenMenuWidget.h"
#include "LimenMainMenuWidget.generated.h"


class ULimenGenericModalWidget;
class ULimenSaveData;

DECLARE_MULTICAST_DELEGATE_OneParam(FNewGameRequest, APlayerController*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLoadGameRequest, APlayerController*, const ULimenSaveData*);
DECLARE_MULTICAST_DELEGATE_OneParam(FOptionsRequest, APlayerController*);
DECLARE_MULTICAST_DELEGATE_OneParam(FQuitRequest, APlayerController*);

/**
 * 
 */
UCLASS()
class LIMENPLAYERS_API ULimenMainMenuWidget : public ULimenMenuWidget
{
	GENERATED_BODY()


public:
	FNewGameRequest OnNewGameRequested; 
	FLoadGameRequest OnLoadGameRequested; 
	FOptionsRequest OnOptionsRequested; 
	FQuitRequest OnQuitRequested;

protected:
	UFUNCTION(BlueprintCallable, Category="Limen|Main Menu")
	void StartNewGame();
	UFUNCTION(BlueprintCallable, Category="Limen|Main Menu")
	void StartLoadGame(const ULimenSaveData* SaveData);
	UFUNCTION(BlueprintCallable, Category="Limen|Main Menu")
	void OptionsButtonPressed();
	UFUNCTION(BlueprintCallable, Category="Limen|Main Menu")
	void QuitButtonPressed();

private:
	UFUNCTION()
	void QuitGameModalDismissed(ULimenGenericModalWidget* ULimenGenericModalWidget, const bool bAccepted);
	
};
