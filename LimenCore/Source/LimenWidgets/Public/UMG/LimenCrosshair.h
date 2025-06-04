// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/Widget.h"
#include "LimenCrosshair.generated.h"

class SImage;

USTRUCT()
struct FCrosshairParameters
{
	GENERATED_BODY()

	FCrosshairParameters()
	{
		CrosshairColor = FColor::Green;
		CrosshairLength = 100.0f;
		CrosshairGap = 10.0f;
		CrosshairThickness = 8.0f;
		bCenterDot = false;
	}

	UPROPERTY(EditAnywhere, Category="Parameters")
	FColor CrosshairColor;
	UPROPERTY(EditAnywhere, Category="Parameters")
	float CrosshairLength;
	UPROPERTY(EditAnywhere, Category="Parameters")
	float CrosshairGap;
	UPROPERTY(EditAnywhere, Category="Parameters")
	float CrosshairThickness;
	UPROPERTY(EditAnywhere, Category="Parameters")
	bool bCenterDot;
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
	FCrosshairParameters CrosshairStyleParameters;
	
	
	virtual TSharedRef<SWidget> RebuildWidget() override;

private:
	static const FSlateBrush CrosshairBrush;
	static const FSlateBrush CenterDotBrush;
	
	TSharedPtr<SImage> TopLine;
	TSharedPtr<SImage> RightLine;
	TSharedPtr<SImage> BottomLine;
	TSharedPtr<SImage> LeftLine;
	TSharedPtr<SImage> CenterDot;

};
