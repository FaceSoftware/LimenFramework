// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenWidget.h"
#include "LimenMenuWidget.generated.h"

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class LIMENWIDGETS_API ULimenMenuWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	explicit ULimenMenuWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Menus|Player Controller")
	virtual void BindPlayerController(APlayerController* InPlayerController);

	template <typename T = APlayerController>
	T* GetBoundPlayerController() const
	{
		static_assert(TIsDerivedFrom<T, APlayerController>::Value);
		return CastChecked<T>(BoundPlayerController.Get(), ECastCheckedType::NullAllowed);
	}

protected:
	UPROPERTY(BlueprintReadOnly, Category="Limen|Menus|Player Controller")
	TObjectPtr<APlayerController> BoundPlayerController;
	
	UFUNCTION(BlueprintNativeEvent, Category="Limen|Menus|Player Controller")
	void OnPlayerControllerBound(APlayerController* InPlayerController);
	virtual void OnPlayerControllerBound_Implementation(APlayerController* InPlayerController) {}
};
