// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenWidget.h"
#include "LimenSubtitleDisplay.generated.h"


class ULimenSubtitle;

typedef TMap<ULimenSubtitle*, FTimerHandle> FSubtitleMap;
typedef TPair<ULimenSubtitle*, FTimerHandle> FSubtitlesPair;

/**
 * 
 */
UCLASS()
class LIMENSUBTITLES_API ULimenSubtitleDisplay : public ULimenWidget
{
	GENERATED_BODY()

public:
	virtual void BeginDestroy() override;
	void AddSubtitle(ULimenSubtitle* InSubtitle);
	void RemoveSubtitle(ULimenSubtitle* InSubtitle);

protected:
	virtual void SubtitleDismissed(ULimenSubtitle* Subtitle);
	
	UFUNCTION(BlueprintImplementableEvent)
	void AddToScreen(ULimenSubtitle* NewSubtitle);
	
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveFromScreen(ULimenSubtitle* NewSubtitle);	

	UFUNCTION(BlueprintCallable)
	TArray<ULimenSubtitle*> GetSortedSubtitles() const;
	
private:
	UPROPERTY()
	TArray<ULimenSubtitle*> ActiveSubtitles;
};
