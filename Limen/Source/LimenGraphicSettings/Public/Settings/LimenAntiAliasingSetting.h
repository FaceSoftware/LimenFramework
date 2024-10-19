// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenAntiAliasingSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenAntiAliasingSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	ULimenAntiAliasingSetting();
	
protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;
	
private:
	const FString FXAA		= TEXT("Fast Aproximate Anti-Aliasing (FXAA)");
	const FString TAA		= TEXT("Temporal Anti-Aliasing (TAA)");
	const FString MSAA		= TEXT("Multisample Anti-Aliasing (MSAA)");
	const FString TSR		= TEXT("Temporal Super-Resolution (TSR)");

	uint8 UnFormatAntiAliasingMode(const FString& Mode) const;
	FString FormatAntiAliasingMode(const uint8 Mode) const;

};
