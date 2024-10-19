// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenMenuWidget.h"
#include "LimenMainMenuWidget.generated.h"


class ULimenSaveData;
class ULimenGameParameters;

DECLARE_MULTICAST_DELEGATE_TwoParams(FNewGameRequest, APlayerController*, ULimenGameParameters*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FLoadGameRequest, APlayerController*, ULimenSaveData*);
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
	void StartNewGame(TSoftObjectPtr<ULimenGameParameters> GameParams);
	UFUNCTION(BlueprintCallable, Category="Limen|Main Menu")
	void StartLoadGame(ULimenSaveData* SaveGame);
	UFUNCTION(BlueprintCallable, Category="Limen|Main Menu")
	void OptionsButtonPressed();
	UFUNCTION(BlueprintCallable, Category="Limen|Main Menu")
	void QuitButtonPressed();

private:
	UFUNCTION()
	void QuitGameModalDismissed(const bool bAccepted);
	
};
