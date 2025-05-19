// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Framework/Text/TextLayout.h"
#include "LimenStandardButton.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLGButtonClicked, ULimenStandardButton*, Button);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLGButtonHover, ULimenStandardButton*, Button);


/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenStandardButton : public UWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLGButtonClicked OnButtonClickedEvent;
	UPROPERTY(BlueprintAssignable)
	FLGButtonHover OnButtonHoveredEvent;
	UPROPERTY(BlueprintAssignable)
	FLGButtonHover OnButtonUnhoveredEvent;

	ULimenStandardButton();
	
	UFUNCTION(BlueprintCallable)
	virtual void SetButtonText(const FText& NewText);
	UFUNCTION(BlueprintCallable)
	virtual void SetButtonStyle(const FButtonStyle& NewStyle);
	UFUNCTION(BlueprintCallable)
	virtual void SetButtonNormalBrush(const FSlateBrush& NewBrush);
	
	UFUNCTION(BlueprintCallable)
	const FGuid& GetButtonId() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Button")
	FButtonStyle ButtonStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Text")
	FText Text;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Text")
	TEnumAsByte<EHorizontalAlignment> TextHorizontalAlignment;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Text")
	TEnumAsByte<EVerticalAlignment> TextVerticalAlignment;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Text")
	FSlateFontInfo Font;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Text")
	FSlateColor TextColor;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Text")
	FMargin TextMargin;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LG|Text")
	TEnumAsByte<ETextJustify::Type> TextJustification;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(const bool bReleaseChildren) override;

	virtual FReply OnClicked();
	virtual void OnHovered();
	virtual void OnUnhovered();

	TSharedPtr<STextBlock> TextBlock;
	
private:
	FGuid ButtonId;
	
};
