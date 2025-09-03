// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LimenWidget.generated.h"


/// Widget Levels
/// 0 to 9 - HUD
/// 10 to 19 - Menus
/// 20 to 39 - Popups
/// 40 to 49 - Loading Screens

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLimenWidgetVisibilityChanged, const bool, bIsVisible);
DECLARE_DYNAMIC_DELEGATE(FLimenBlueprintWidgetHidden);
DECLARE_DELEGATE(FLimenWidgetHidden);

UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENWIDGETS_API ULimenWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	template<typename T>
	FORCEINLINE static T* IsWidgetValid(UUserWidget* TestWidget, APlayerController* PlayerController, const TSubclassOf<ULimenWidget> WidgetClass, const bool bCreateIfNot)
	{
		if (IsValid(TestWidget))
		{
			return Cast<T>(TestWidget);
		}
		
		if (!bCreateIfNot)
		{
			return nullptr;
		}

		auto* OutWidget = CreateWidget(PlayerController, WidgetClass);
		check(IsValid(OutWidget))

		return Cast<T>(OutWidget);
	}

public:
	UPROPERTY(BlueprintAssignable, Category="Limen|Widgets")
	FLimenWidgetVisibilityChanged OnLimenVisibilityChanged;
	
	UPROPERTY(BlueprintAssignable, Category="Limen|Widgets")
	FLimenWidgetVisibilityChanged OnLimenAnimationFinished;

	explicit ULimenWidget(const FObjectInitializer& ObjectInitializer);
	virtual void NativeConstruct() override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Widgets")
	bool ShowWidget();
	UFUNCTION(BlueprintCallable, Category="Limen|Widgets")
	bool HideWidget();
	UFUNCTION(BlueprintCallable, Category="Limen|Widgets")
	void HideWidgetWithCallback(FLimenBlueprintWidgetHidden OnWidgetHidden);
	void HideWidget(const FLimenWidgetHidden& OnWidgetHidden);
	
	void ToggleWidgetVisibility();
	UFUNCTION(BlueprintCallable, Category="Limen|Widgets")
	virtual bool IsShowing() const;
	UFUNCTION(BlueprintCallable, Category="Limen|Widgets")
	virtual bool IsHiding() const;
	bool IsAnimating() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Widget")
	void DestroyWidget(const bool bWaitForHideAnimation = true);

protected:
	UPROPERTY(EditAnywhere, Category="Limen|Animations", Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> ShowAnimation;
	UPROPERTY(EditAnywhere, Category="Limen|Animations", Transient, meta=(BindWidgetAnimOptional))
	TObjectPtr<UWidgetAnimation> HideAnimation;

	UPROPERTY(EditAnywhere, Category="Limen|Animations")
	ESlateVisibility DefaultVisibleState;
	UPROPERTY(EditAnywhere, Category="Limen|Animations")
	ESlateVisibility DefaultHiddenState;

	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Widgets", BlueprintCosmetic)
	void PlayOnWidgetVisibleAnimation();
	UFUNCTION(BlueprintNativeEvent, Category="Limen|Widgets")
	void OnWidgetVisible();
	virtual void OnWidgetVisible_Implementation() {}

	UFUNCTION(BlueprintImplementableEvent, Category="Limen|Widgets", BlueprintCosmetic)
	void PlayOnWidgetHiddenAnimation();
	UFUNCTION(BlueprintNativeEvent, Category="Limen|Widgets")
	void OnWidgetHidden();
	virtual void OnWidgetHidden_Implementation() {}

	/**
	 * @brief Sets the widget ZOrder. Only call this in the constructor.
	 * @param NewLevel 
	 */
	void SetWidgetLevel(const int32 NewLevel);
	uint8 GetWidgetLevel() const;

	void SetDefaultVisibleState(const ESlateVisibility NewDefaultVisibleState);
	void SetDefaultHiddenState(const ESlateVisibility NewDefaultHiddenState);

	ESlateVisibility GetDefaultVisibleState() const;
	ESlateVisibility GetDefaultHiddenState() const;

private:
	bool bIsAnimating;
	int32 WidgetLevel = 0;
	FWidgetAnimationHandle ShowAnimationHandle;
	FWidgetAnimationHandle HideAnimationHandle;

	/**
	 * @brief Defines how this widget is added to the viewport.
	 * The default behavior is calling AddToViewport.
	 */
	virtual void ShowWidgetMethod();
	virtual void HideWidgetMethod();

	void ShowAllChildren();
	void HideAllChildren();

	FLimenBlueprintWidgetHidden CurrentBlueprintDelegate;
	FLimenWidgetHidden CurrentDelegate;
	UFUNCTION()
	void WidgetAnimationEnd_Internal(const bool bVisible);
	UFUNCTION()
	void WidgetVisibilityChanged_Internal(const bool bVisible);

	UFUNCTION()
	void DestroyWidgetInternal(const bool bIsHideAnimation = true);

	FORCEINLINE void ShowAnimationFinished(FWidgetAnimationState& State);
	FORCEINLINE void ShowAnimationFinished();
	FORCEINLINE void HideAnimationFinished(FWidgetAnimationState& State);
	FORCEINLINE void HideAnimationFinished();
};
