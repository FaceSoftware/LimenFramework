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
const FSlateBrush ULimenCrosshair::CenterDotBrush = []
{
	FSlateBrush Result;
	Result.DrawAs = ESlateBrushDrawType::Type::RoundedBox;
	Result.OutlineSettings.bUseBrushTransparency = true;
	Result.OutlineSettings.RoundingType = ESlateBrushRoundingType::HalfHeightRadius;
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
	TSharedRef<SOverlay> Root = SNew(SOverlay);

	// Top Line
	{
		TopLine = SNew(SImage)
			.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
			.Image(&CrosshairBrush)
			.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairThickness,
										   CrosshairStyleParameters.CrosshairLength));
		Root->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, 0.f, 0.f, CrosshairStyleParameters.CrosshairGap))
		[
			TopLine.ToSharedRef()
		];
	}

	// Bottom Line
	{
		BottomLine = SNew(SImage)
			.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
			.Image(&CrosshairBrush)
			.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairThickness,
										   CrosshairStyleParameters.CrosshairLength));
		Root->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, CrosshairStyleParameters.CrosshairGap, 0.f, 0.f))
		[
			BottomLine.ToSharedRef()
		];
	}

	// Left Line
	{
		LeftLine = SNew(SImage)
			.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
			.Image(&CrosshairBrush)
			.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairLength,
										   CrosshairStyleParameters.CrosshairThickness));
		Root->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(CrosshairStyleParameters.CrosshairGap, 0.f, 0.f, 0.f))
		[
			LeftLine.ToSharedRef()
		];
	}

	// Right Line
	{
		RightLine = SNew(SImage)
			.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
			.Image(&CrosshairBrush)
			.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairLength,
										   CrosshairStyleParameters.CrosshairThickness));
		Root->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, 0.f, CrosshairStyleParameters.CrosshairGap, 0.f))
		[
			RightLine.ToSharedRef()
		];
	}

	// CenterDot
	if (CrosshairStyleParameters.bCenterDot)
	{
		CenterDot = SNew(SImage)
			.ColorAndOpacity(CrosshairStyleParameters.CrosshairColor)
			.Image(&CenterDotBrush)
			.DesiredSizeOverride(FVector2D(CrosshairStyleParameters.CrosshairThickness,
										   CrosshairStyleParameters.CrosshairThickness));
		Root->AddSlot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		.Padding(FMargin(0.f, 0.f, 0.f, 0.f))
		[
			CenterDot.ToSharedRef()
		];
	}

	return Root;
}
