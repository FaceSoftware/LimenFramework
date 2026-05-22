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
	virtual void ApplyCurrentSetting(bool bUserRequest) override;
	virtual void SetDefaults() override;
	
	virtual bool IsMSAASupported() const;
	
private:
	TArray<TConsoleSetting<int32>> SettingsDescription;
};
