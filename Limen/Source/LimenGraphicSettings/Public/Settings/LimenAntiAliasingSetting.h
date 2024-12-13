// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenAntiAliasingSetting.generated.h"

class ULimenUpscalingSetting;
/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenAntiAliasingSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()

public:
	static const FString NONE;
	static const FString FXAA;
	static const FString TAA;
	static const FString MSAA;
	static const FString TSR;
	
	ULimenAntiAliasingSetting();
	
protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;
	
private:
	uint8 UnFormatAntiAliasingMode(const FString& Mode) const;
	FString FormatAntiAliasingMode(const uint8 Mode) const;
};
