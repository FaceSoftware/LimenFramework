// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LimenLoadingScreenParameters.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, NotBlueprintType)
class LIMENLEVELTRANSITIONS_API ULimenLoadingScreenParameters : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<ULimenLoadingScreenWidget> LoadingScreenWidget;
	UPROPERTY(EditDefaultsOnly)
	float MinimumDisplayTimeSeconds = 0.f;
	UPROPERTY(EditDefaultsOnly)
	float PostCompleteDisplayTime = 0.f;
	
	TSubclassOf<UUserWidget> GetLoadingScreenWidgetClass() const
	{
		return LoadingScreenWidget.LoadSynchronous();
	}

	float GetMinimumDisplayTimeSeconds() const
	{
		return MinimumDisplayTimeSeconds;
	}
	float GetPostCompleteDisplayTime() const
	{
		return MinimumDisplayTimeSeconds;
	}
};
