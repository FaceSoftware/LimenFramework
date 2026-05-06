// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/HUD.h"
#include "Widgets/LimenWidget.h"
#include "LimenBaseHUD.generated.h"


class ULimenMenuWidget;
class ULimenBaseHudWidget;
class UWidget;
class ULimenLoadingScreenWidget;
class ULimenNotificationComponent;
struct FNotificationParams;
class ULimenWidget;


#define DECLARE_WIDGET_ACCESSORS(Type, WidgetVarName) \
	FORCEINLINE void Show##WidgetVarName() { ShowWidget_Internal(WidgetVarName.Get()); } \
	FORCEINLINE void Hide##WidgetVarName() { HideWidget_Internal(WidgetVarName.Get()); } \
	FORCEINLINE bool Is##WidgetVarName##Visible() const { return WidgetVarName && WidgetVarName->IsShowing(); } \
	template<typename T = ULimenWidget> \
	FORCEINLINE T* Get##WidgetVarName() const \
	{ \
		static_assert(TIsDerivedFrom<T, ULimenWidget>::IsDerived); \
		return Cast<T>(WidgetVarName.Get()); \
	} \

#define CREATE_MENU_WIDGET_STRONG(Typename, WidgetPtr, WidgetClass) \
	if (!WidgetClass.IsNull()) \
	{ \
		auto* Widget = CreateWidget<Typename>(GetOwningPlayerController(), WidgetClass.LoadSynchronous()); \
		WidgetPtr = TStrongObjectPtr(Widget); \
		WidgetPtr->BindPlayerController(GetOwningPlayerController()); \
		AddWidgetToList(Widget); \
	} \

#define CREATE_HUD_WIDGET_STRONG(Typename, WidgetPtr, WidgetClass) \
	CREATE_MENU_WIDGET_STRONG(Typename, WidgetPtr, WidgetClass) \

#define CREATE_WIDGET(Typename, WidgetPtr, WidgetClass) \
	if (!WidgetClass.IsNull()) \
	{ \
		WidgetPtr = CreateWidget<Typename>(GetOwningPlayerController(), WidgetClass.LoadSynchronous()); \
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
	virtual void ShowHUD() override;
	
	virtual void HideHUD();
	void ForceHUDState(const bool bForce);
	bool GetForcedHudState() const;
	void ShowActiveWidget();
	void HideActiveWidget();
	void UpdateWidgets(APlayerController* PlayerController, APawn* Pawn);
	virtual ULimenWidget* GetActiveWidget() const;
	virtual TArray<TWeakObjectPtr<ULimenWidget>> GetWidgetList() const;
	
protected:
	TWeakObjectPtr<ULimenWidget> ActiveWidget;
	
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void DestroyWidgets();
	virtual void InitializeWidgets();
	virtual void HudInitialized(APlayerController* PlayerController);
	virtual bool CanSwitchWidgetVisibility(UWidget* InWidget) const;
	void ShowWidget_Internal(ULimenWidget* Widget);
	void HideWidget_Internal(ULimenWidget* Widget);
	virtual void AddWidgetToList(ULimenWidget* InWidget);
	virtual void RemoveWidgetFormList(ULimenWidget* InWidget);
	
	template<typename T>
	T* CreateHudWidget(UClass* InWidgetClass)
	{
		static_assert(TIsDerivedFrom<T, ULimenBaseHudWidget>::IsDerived);
		if (InWidgetClass)
		{
			T* Widget = CreateWidget<T>(GetOwningPlayerController(), InWidgetClass);
			Widget->BindPlayerController(GetOwningPlayerController());
			AddWidgetToList(Widget);
			return Widget;
		}
		return nullptr;
	}
	template<typename T>
	TStrongObjectPtr<T> CreateHudWidgetStrongPtr(UClass* InWidgetClass)
	{
		static_assert(TIsDerivedFrom<T, ULimenBaseHudWidget>::IsDerived);
		T* Widget = CreateHudWidget<T>(InWidgetClass);
		return TStrongObjectPtr<T>(Widget);
	}
	template<typename T>
	TStrongObjectPtr<T> CreateHudWidgetStrongPtr(const TSoftClassPtr<T>& InWidgetClass)
	{
		static_assert(TIsDerivedFrom<T, ULimenBaseHudWidget>::IsDerived);
		return TStrongObjectPtr(CreateHudWidget<T>(InWidgetClass.LoadSynchronous()));
	}
	template<typename T>
	T* CreateMenuWidget(UClass* InWidgetClass)
	{
		static_assert(TIsDerivedFrom<T, ULimenMenuWidget>::IsDerived);
		if (InWidgetClass)
		{
			T* Widget = CreateWidget<T>(GetOwningPlayerController(), InWidgetClass);
			Widget->BindPlayerController(GetOwningPlayerController());
			AddWidgetToList(Widget);
			return Widget;
		}
		return nullptr;
	}
	template<typename T>
	TStrongObjectPtr<T> CreateMenuWidgetStrongPtr(UClass* InWidgetClass)
	{
		static_assert(TIsDerivedFrom<T, ULimenMenuWidget>::IsDerived);
		return TStrongObjectPtr(CreateMenuWidget<T>(InWidgetClass));
	}
	template<typename T>
	TStrongObjectPtr<T> CreateMenuWidgetStrongPtr(const TSoftClassPtr<T>& InWidgetClass)
	{
		static_assert(TIsDerivedFrom<T, ULimenMenuWidget>::IsDerived);
		return TStrongObjectPtr(CreateMenuWidget<T>(InWidgetClass.LoadSynchronous()));
	}
	template<typename T>
	void CleanupWidget(T* InWidget)
	{
		static_assert(TIsDerivedFrom<T, ULimenWidget>::IsDerived);
		if (InWidget)
		{
			RemoveWidgetFormList(InWidget);
			InWidget->DestroyWidget(false);
		}
	}
	template<typename T>
	void CleanupWidget(TStrongObjectPtr<T>& InWidget)
	{
		static_assert(TIsDerivedFrom<T, ULimenWidget>::IsDerived);
		CleanupWidget(InWidget.Get());
		InWidget.Reset();
	}

private:
	uint8 bAreWidgetsInitialized : 1;
	uint8 bForceHideHud : 1;
	
	TArray<TWeakObjectPtr<ULimenWidget>> WidgetList;
};
