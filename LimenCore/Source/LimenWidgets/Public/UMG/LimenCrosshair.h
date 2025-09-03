// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "Widgets/SLeafWidget.h"
#include "LimenCrosshair.generated.h"

class SImage;

USTRUCT(BlueprintType)
struct LIMENWIDGETS_API FCrosshairStyleParameters
{
	GENERATED_BODY()

	FCrosshairStyleParameters()
	{
		CrosshairColor = FColor::Green;
		CrosshairLength = 100.0f;
		CrosshairGap = 10.0f;
		CrosshairThickness = 8.0f;
		bCenterDot = false;

		CrosshairBrush.DrawAs = ESlateBrushDrawType::Type::Box;
		CrosshairBrush.ImageType = ESlateBrushImageType::NoImage;
		CrosshairBrush.ImageSize = FVector2D(1.0f, 1.0f);
		CrosshairBrush.Tiling = ESlateBrushTileType::Type::Both;
		CrosshairBrush.TintColor = FSlateColor(FColor::White);

		CenterDotBrush.DrawAs = ESlateBrushDrawType::Type::RoundedBox;
		CenterDotBrush.OutlineSettings.bUseBrushTransparency = true;
		CenterDotBrush.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
		CenterDotBrush.ImageType = ESlateBrushImageType::NoImage;
		CenterDotBrush.ImageSize = FVector2D(1.0f, 1.0f);
		CenterDotBrush.Tiling = ESlateBrushTileType::Type::Both;
		CenterDotBrush.TintColor = FSlateColor(FColor::White);
	}

	UPROPERTY(EditAnywhere, Category="Parameters")
	FLinearColor CrosshairColor;
	UPROPERTY(EditAnywhere, Category="Parameters")
	float CrosshairLength;
	UPROPERTY(EditAnywhere, Category="Parameters")
	float CrosshairGap;
	UPROPERTY(EditAnywhere, Category="Parameters")
	float CrosshairThickness;
	UPROPERTY(EditAnywhere, Category="Parameters")
	bool bCenterDot;
	UPROPERTY(EditAnywhere, Category="Parameters")
	FSlateBrush CrosshairBrush;
	UPROPERTY(EditAnywhere, Category="Parameters", meta=(EditCondition="bCenterDot", EditConditionHides="true"))
	FSlateBrush CenterDotBrush;
};

class LIMENWIDGETS_API SLimenCrosshair : public SLeafWidget
{
public:
	SLATE_BEGIN_ARGS(SLimenCrosshair) { }
		SLATE_ATTRIBUTE(FCrosshairStyleParameters, StyleParams)
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual FVector2D ComputeDesiredSize(float) const override;
	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

private:
	FCrosshairStyleParameters Style;
};

/**
 * 
 */
UCLASS()
class LIMENWIDGETS_API ULimenCrosshair : public UWidget
{
	GENERATED_BODY()

public:
	ULimenCrosshair();

protected:
	UPROPERTY(EditAnywhere, Category="Crosshair")
	FCrosshairStyleParameters CrosshairStyleParameters;

	virtual TSharedRef<SWidget> RebuildWidget() override;

private:	
	TSharedPtr<SImage> TopLine;
	TSharedPtr<SImage> RightLine;
	TSharedPtr<SImage> BottomLine;
	TSharedPtr<SImage> LeftLine;
	TSharedPtr<SImage> CenterDot;

};
