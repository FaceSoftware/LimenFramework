// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "GameFramework/HUD.h"
#include "LimenBaseHUD.generated.h"


class UWidget;
class ULimenLoadingScreenWidget;
class ULimenNotificationComponent;
struct FNotificationParams;
class ULimenWidget;

/**
 * 
 */
UCLASS(Blueprintable)
class LIMENGAMEFRAMEWORK_API ALimenBaseHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FObjectInitializationDelegate OnHudInitialized;
	
	ALimenBaseHUD();
	virtual void ShowHUD() override;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void ForceHUDState(const bool bForce);
	bool GetForcedHudState() const;
	
	void ShowActiveWidget();
	void HideActiveWidget();

	/**
	 * @brief Queues a notification for this HUD.
	 * @param InParams 
	 */
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	virtual void QueueNotification(const FNotificationParams& InParams);

	/**
	 * @brief Pure virtual.
	 * @param PlayerController 
	 * @param Pawn 
	 */
	void UpdateWidgets(APlayerController* PlayerController, APawn* Pawn);

	ULimenNotificationComponent* GetNotificationsComponent() const;

	virtual ULimenWidget* GetActiveWidget() const;
	
protected:
	
	UPROPERTY()
	TObjectPtr<ULimenWidget> ActiveWidget;

	virtual void DestroyWidgets();
	virtual void InitializeWidgets();

	virtual void HudInitialized(APlayerController* PlayerController) {}
	UFUNCTION(BlueprintImplementableEvent, Category="Limen|HUD", DisplayName="Hud Initialized")
	void BP_HudInitialized(APlayerController* PlayerController);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPostProcessEnabled();
	UFUNCTION(BlueprintImplementableEvent)
	void OnPostProcessDisabled();

	UFUNCTION(BlueprintCallable)
	virtual bool CanSwitchWidgetVisibility(UWidget* InWidget) const;

	void ShowWidget_Internal(ULimenWidget* Widget);
	void HideWidget_Internal(ULimenWidget* Widget);

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULimenNotificationComponent> NotificationComponent;
	
	uint8 bAreWidgetsInitialized : 1;
	uint8 bForceHideHud : 1;
	
#if WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category="Limen|Debug")
	bool bDisableLoadingScreenInPIE;

#endif
	
};
