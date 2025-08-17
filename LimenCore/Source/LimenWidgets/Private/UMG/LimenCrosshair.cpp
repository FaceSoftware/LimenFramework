// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenCrosshair.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SOverlay.h"


void SLimenCrosshair::Construct(const FArguments& InArgs)
{
	Style = InArgs._StyleParams.Get();
}

FVector2D SLimenCrosshair::ComputeDesiredSize(float) const
{
	return FVector2D(Style.CrosshairLength);
}

int32 SLimenCrosshair::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
							   const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
							   const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	const bool bEnabled = ShouldBeEnabled(bParentEnabled);
	const ESlateDrawEffect DrawEffects = bEnabled ? ESlateDrawEffect::None : ESlateDrawEffect::DisabledEffect;

	const FVector2D WidgetSize = AllottedGeometry.GetLocalSize();
	const FVector2D Center = WidgetSize * 0.5f;

	const float L  = Style.CrosshairLength;
	const float T  = Style.CrosshairThickness;
	const float G  = Style.CrosshairGap;
	const float LineLength = (L - G) * 0.5;

	// Colors respect parent/widget opacity tints
	const FLinearColor Tint = Style.CrosshairColor * InWidgetStyle.GetColorAndOpacityTint();

	// Reusable 1px box brush (no texture)
	static FSlateBrush BoxBrush;
	static bool bInit = false;
	if (!bInit)
	{
		BoxBrush.DrawAs = ESlateBrushDrawType::Box;
		BoxBrush.ImageType = ESlateBrushImageType::NoImage;
		BoxBrush.Tiling = ESlateBrushTileType::Both;
		BoxBrush.ImageSize = FVector2D(1,1);
		bInit = true;
	}

	auto DrawRect = [&](const FVector2D& Pos, const FVector2D& RectSize, const FSlateBrush* Brush, const FLinearColor& Color, int32 Z)
	{
		FSlateDrawElement::MakeBox(
			OutDrawElements, Z,
			AllottedGeometry.ToPaintGeometry(RectSize, FSlateLayoutTransform(Pos)),
			Brush, DrawEffects, Color
		);
	};

	// Top Line
	{
		const FVector2D Size(Style.CrosshairThickness, LineLength);
		const FVector2D Pos(Center.X - T * 0.5f, 0.f);
		if (!Size.IsZero() && !Size.ContainsNaN()) DrawRect(Pos, Size, &BoxBrush, Tint, LayerId);
	}

	// Bottom Line
	{
		const FVector2D Size(Style.CrosshairThickness, LineLength);
		const FVector2D Pos(Center.X - T * 0.5f, WidgetSize.Y - LineLength);
		if (!Size.IsZero() && !Size.ContainsNaN()) DrawRect(Pos, Size, &BoxBrush, Tint, LayerId);
	}

	// Left Line
	{
		const FVector2D Size(LineLength, Style.CrosshairThickness);
		const FVector2D Pos(0.f, Center.Y - T * 0.5f);
		if (!Size.IsZero() && !Size.ContainsNaN()) DrawRect(Pos, Size, &BoxBrush, Tint, LayerId);
	}

	// Right Line
	{
		const FVector2D Size(LineLength, Style.CrosshairThickness);
		const FVector2D Pos(WidgetSize.X - LineLength, Center.Y - T * 0.5f);
		if (!Size.IsZero() && !Size.ContainsNaN()) DrawRect(Pos, Size, &BoxBrush, Tint, LayerId);
	}

	// CenterDot
	if (Style.bCenterDot)
	{
		const FVector2D Size(Style.CrosshairThickness, Style.CrosshairThickness);
		const FVector2D Pos(Center.X - T * 0.5f, Center.Y - T * 0.5f);
		if (!Size.IsZero() && !Size.ContainsNaN()) DrawRect(Pos, Size, &BoxBrush, Tint, LayerId);
	}

	return LayerId + 1;
}

ULimenCrosshair::ULimenCrosshair()
{
}

TSharedRef<SWidget> ULimenCrosshair::RebuildWidget()
{

	FCrosshairStyleParameters P;
	P.CrosshairLength = CrosshairStyleParameters.CrosshairLength;
	P.CrosshairThickness = CrosshairStyleParameters.CrosshairThickness;
	P.CrosshairGap = CrosshairStyleParameters.CrosshairGap;
	P.bCenterDot = CrosshairStyleParameters.bCenterDot;
	P.CrosshairColor = CrosshairStyleParameters.CrosshairColor;

	return SNew(SLimenCrosshair).StyleParams(P);
}
