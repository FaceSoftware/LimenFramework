// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "LimenBaseHUD.generated.h"


class UWidget;
class ULimenLoadingScreenWidget;
class ULimenNotificationComponent;
struct FNotificationParams;
class ULimenWidget;


#define DECLARE_WIDGET_ACCESSORS(Type, WidgetVarName) \
	FORCEINLINE void Show##WidgetVarName() { ShowWidget_Internal(WidgetVarName.Get()); } \
	FORCEINLINE void Hide##WidgetVarName() { HideWidget_Internal(WidgetVarName.Get()); } \
	FORCEINLINE bool Is##WidgetVarName##Visible() const { return WidgetVarName && WidgetVarName->IsShowing(); } \
	FORCEINLINE Type* Get##WidgetVarName() const { return WidgetVarName.Get(); } \

#define CREATE_MENU_WIDGET_STRONG(Typename, WidgetPtr, WidgetClass) \
	if (WidgetClass) \
	{ \
		auto* Widget = CreateWidget<Typename>(GetOwningPlayerController(), WidgetClass); \
		WidgetPtr = TStrongObjectPtr(Widget); \
		WidgetPtr->BindPlayerController(GetOwningPlayerController()); \
		AddWidgetToList(Widget); \
	} \

#define CREATE_HUD_WIDGET_STRONG(Typename, WidgetPtr, WidgetClass) \
	if (WidgetClass) \
	{ \
		auto* Widget = CreateWidget<Typename>(GetOwningPlayerController(), WidgetClass); \
		WidgetPtr = TStrongObjectPtr(Widget); \
		WidgetPtr->BindPlayerController(GetOwningPlayerController()); \
		AddWidgetToList(Widget); \
	} \

#define CREATE_WIDGET(Typename, WidgetPtr, WidgetClass) \
	if (WidgetClass) \
	{ \
		WidgetPtr = CreateWidget<Typename>(GetOwningPlayerController(), WidgetClass); \
		AddWidgetToList(Widget); \
	} \

#define CLEANUP_WIDGET_STRONG(WidgetPtr) \
	if (WidgetPtr) \
	{ \
		RemoveWidgetFormList(WidgetPtr.Get()); \
		WidgetPtr->DestroyWidget(false); \
		WidgetPtr.Reset(); \
	} \

/**
 * 
 */
UCLASS(Blueprintable)
class LIMENGAMEFRAMEWORK_API ALimenBaseHUD : public AHUD
{
	GENERATED_BODY()
	
public:	
	ALimenBaseHUD();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void ShowHUD() override;
	virtual void HideHUD();

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
	virtual TArray<TWeakObjectPtr<ULimenWidget>> GetWidgetList() const;
	
protected:
	
	UPROPERTY()
	TObjectPtr<ULimenWidget> ActiveWidget;

	virtual void DestroyWidgets();
	virtual void InitializeWidgets();

	virtual void HudInitialized(APlayerController* PlayerController);

	UFUNCTION(BlueprintCallable)
	virtual bool CanSwitchWidgetVisibility(UWidget* InWidget) const;

	void ShowWidget_Internal(ULimenWidget* Widget);
	void HideWidget_Internal(ULimenWidget* Widget);
	virtual void AddWidgetToList(ULimenWidget* InWidget);
	virtual void RemoveWidgetFormList(ULimenWidget* InWidget);

private:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULimenNotificationComponent> NotificationComponent;
	
	uint8 bAreWidgetsInitialized : 1;
	uint8 bForceHideHud : 1;
	
#if WITH_EDITORONLY_DATA

	UPROPERTY(EditDefaultsOnly, Category="Limen|Debug")
	bool bDisableLoadingScreenInPIE;

#endif
	
	TArray<TWeakObjectPtr<ULimenWidget>> WidgetList;
};
