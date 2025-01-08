// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/LimenBaseHUD.h"
#include "Widgets/LimenMainMenuWidget.h"
#include "LimenMenuHUD.generated.h"

class ULimenMainMenuWidget;

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class LIMENPLAYERS_API ALimenMenuHUD : public ALimenBaseHUD
{
	GENERATED_BODY()

public:
	ALimenMenuHUD();
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowMainMenuWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HideMainMenuWidget();
	bool IsMainMenuWidgetShowing() const;
	ULimenMainMenuWidget* GetMainMenuWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowOptionsWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HideOptionsWidget();
	bool IsOptionsWidgetShowing() const;
	ULimenMenuWidget* GetOptionsWidget() const;

	
	UFUNCTION(BlueprintCallable)
	const TMap<int32, UTexture2D*>& GetLevelContextBackgrounds() const;
	
protected:
	virtual void DestroyWidgets() override;
	virtual void InitializeWidgets() override;

	UFUNCTION()
	virtual void OnMainMenuVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void OnOptionsMenuVisibilityChanged(const bool bIsVisible);

private:
	UPROPERTY(EditDefaultsOnly, Category="Limen|Classes")
	TSoftClassPtr<ULimenMainMenuWidget> MainMenuWidgetClass;
	UPROPERTY()
	TObjectPtr<ULimenMainMenuWidget> MainMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category="Limen|Classes")
	TSoftClassPtr<ULimenMenuWidget> OptionsWidgetClass;
	UPROPERTY()
	TObjectPtr<ULimenMenuWidget> OptionsWidget;

	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TMap<int32, UTexture2D*> LevelContextBackgrounds;
};
