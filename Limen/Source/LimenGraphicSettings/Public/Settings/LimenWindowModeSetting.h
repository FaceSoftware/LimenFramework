// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Settings/LimenSelectionSetting.h"
#include "LimenWindowModeSetting.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGRAPHICSETTINGS_API ULimenWindowModeSetting : public ULimenSelectionSetting
{
	GENERATED_BODY()
	
public:
	ULimenWindowModeSetting();

protected:
	virtual void ApplyCurrentSetting() override;
	virtual void SetDefaults() override;

private:
	static FString FormatWindowMode(const EWindowMode::Type InType);
	static EWindowMode::Type UnFormatWindowMode(const FString& InWindowMode);
};
