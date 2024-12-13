// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "Actors/LimenGameplayActor.h"
#include "LimenBaseHUD.generated.h"


class ULimenLoadingScreenWidget;
class ULimenNotificationComponent;
struct FNotificationParams;
class ULimenWidget;

/**
 * 
 */
UCLASS(Abstract, NotBlueprintable)
class LIMENPLAYERS_API ALimenBaseHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FObjectInitializationDelegate OnHudInitialized;
	
	ALimenBaseHUD();
	
	void HideActiveWidget();
	UUserWidget* GetActiveWidget() const;
	template<typename T> T* GetActiveWidget() const;
	virtual bool InitializeWidget(const TSubclassOf<UUserWidget>& WidgetClass);
	
protected:
	UPROPERTY()
	TObjectPtr<ULimenWidget> ActiveWidget;

	UFUNCTION(BlueprintCallable)
	virtual bool CanSwitchWidgetsVisibility() const;

	void ShowWidget_Internal(ULimenWidget* Widget);
	void HideWidget_Internal(ULimenWidget* Widget);

private:	
	uint8 bAreWidgetsInitialized : 1;	
};

template <typename T>
T* ALimenBaseHUD::GetActiveWidget() const
{
	return Cast<T>(GetActiveWidget());
}
