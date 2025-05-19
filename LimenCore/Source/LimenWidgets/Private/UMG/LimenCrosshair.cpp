// Copyright FaceSoftware. All Rights Reserved.


#include "UMG/LimenCrosshair.h"

#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SOverlay.h"
#include "Widgets/Images/SImage.h"


const FSlateBrush ULimenCrosshair::CrosshairBrush = []
{
	FSlateBrush Result;
	Result.DrawAs = ESlateBrushDrawType::Type::Box;
	Result.ImageType = ESlateBrushImageType::NoImage;
	Result.ImageSize = FVector2D(1.0f, 1.0f);
	Result.Tiling = ESlateBrushTileType::Type::Both;
	Result.TintColor = FSlateColor(FColor::White);
	return Result;
}();

ULimenCrosshair::ULimenCrosshair()
{
}

TSharedRef<SWidget> ULimenCrosshair::RebuildWidget()
{
	// return Super::RebuildWidget();

	// Top Line
	TopLine = SNew(SImage)
		.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
		.Image(&CrosshairBrush)
		.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairThickness,
									   CrosshairStyleParameters.CrosshairLength));

	// Bottom Line
	BottomLine = SNew(SImage)
		.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
		.Image(&CrosshairBrush)
		.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairThickness,
									   CrosshairStyleParameters.CrosshairLength));

	// Left Line
	LeftLine = SNew(SImage)
		.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
		.Image(&CrosshairBrush)
		.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairLength,
									   CrosshairStyleParameters.CrosshairThickness));

	// Right Line
	RightLine = SNew(SImage)
		.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
		.Image(&CrosshairBrush)
		.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairLength,
									   CrosshairStyleParameters.CrosshairThickness));

	TSharedRef<SWidget> Root = SNew(SOverlay)
		// Top Line Slot
		+SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, 0.f, 0.f, CrosshairStyleParameters.CrosshairGap))
		[
			TopLine.ToSharedRef()
		]
		// Bottom Line
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, CrosshairStyleParameters.CrosshairGap, 0.f, 0.f))
		[
			BottomLine.ToSharedRef()
		]
		// Left Line
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(CrosshairStyleParameters.CrosshairGap, 0.f, 0.f, 0.f))
		[
			LeftLine.ToSharedRef()
		]
		// Right Line
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, 0.f, CrosshairStyleParameters.CrosshairGap, 0.f))
		[
			RightLine.ToSharedRef()
		];

	return Root;
}
