// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "HUD/LimenBaseHUD.h"
#include "LimenHUD.generated.h"


class ULimenHintWidget;
class ULimenItemSmithWrapperWidget;
class ULimenDeathScreen;
class ULimenPauseMenuWidget;
class ULimenHudWidget;
class ULimenGameMenuWrapperWidget;
class ULimenLoadingScreenWidget;

/**
 * 
 */
UCLASS(Blueprintable)
class LIMENPLAYERS_API ALimenHUD : public ALimenBaseHUD
{
	GENERATED_BODY()

public:
	ALimenHUD();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowGameMenuWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HideGameMenuWidget();
	void ToggleGameMenuWidget();
	bool IsGameMenuShowing() const;
	ULimenGameMenuWrapperWidget* GetGameMenuWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowCharacterHudWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HideCharacterHudWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void ForceHUDState(const bool bForce);
	void ToggleCharacterHudWidget();
	bool IsCharacterHudShowing() const;
	ULimenHudWidget* GetHudWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowPauseMenuWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HidePauseMenuWidget();
	void TogglePauseMenuWidget();
	bool IsPauseMenuShowing() const;
	ULimenPauseMenuWidget* GetPauseMenuWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowDeathScreenWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HideDeathScreenWidget();
	void ToggleDeathScreenWidget();
	bool IsDeathScreenShowing() const;
	ULimenDeathScreen* GetDeathScreenWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void ShowItemSmithWidget();
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void HideItemSmithWidget();
	void ToggleItemSmithWidget();
	bool IsItemSmithShowing() const;
	ULimenItemSmithWrapperWidget* GetItemSmithWidget() const;

	virtual void QueueNotification(const FNotificationParams& InParams) override;
	
	UFUNCTION(BlueprintCallable)
	void SetPostProcessEnabled(const bool bEnable);
	UFUNCTION(BlueprintCallable)
	bool IsPostProcessEnabled() const;
	
protected:
	UPROPERTY(EditDefaultsOnly, Transient)
	bool bShowHudWidgetAfterLoadingScreen;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen|Classes")
	TSoftClassPtr<ULimenGameMenuWrapperWidget> GameMenuWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenGameMenuWrapperWidget> GameMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category="Limen|Classes")
	TSoftClassPtr<ULimenPauseMenuWidget> PauseMenuWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenPauseMenuWidget> PauseMenuWidget;

	UPROPERTY(EditDefaultsOnly, Category="Limen|Classes")
	TSoftClassPtr<ULimenHudWidget> CharacterHudWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenHudWidget> CharacterHudWidget;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen|Classes")
	TSoftClassPtr<ULimenDeathScreen> DeathScreenWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenDeathScreen> DeathScreenWidget;

	UPROPERTY(EditDefaultsOnly, Category="Limen|Classes")
	TSoftClassPtr<ULimenItemSmithWrapperWidget> ItemSmithWrapperWidgetClass;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<ULimenItemSmithWrapperWidget> ItemSmithWrapperWidget;

	UFUNCTION()
	virtual void LoadingScreenVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void CharacterHudVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void GameMenuVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void PauseMenuVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void DeathScreenVisibilityChanged(const bool bIsVisible);
	UFUNCTION()
	virtual void UpgradeShopVisibilityChanged(const bool bIsVisible);
	
	virtual void DestroyWidgets() override;
	virtual void InitializeWidgets() override;

	virtual void HudInitialized(APlayerController* PlayerController) override;
	
	UFUNCTION(BlueprintCallable)
	bool IsAnyAnimationInProgress() const;

	virtual bool CanSwitchWidgetsVisibility() const override;
	
private:		
	UPROPERTY()
	TArray<ULimenWidget*> WidgetsList;

	uint8 bPostProcessEnabled : 1;
	uint8 bForceHUDState : 1;
};
