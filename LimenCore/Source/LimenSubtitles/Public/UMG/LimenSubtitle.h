// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Structs/SubtitleCoreStructs.h"
#include "Widgets/LimenWidget.h"
#include "LimenSubtitle.generated.h"


class ULimenSubtitle;
DECLARE_MULTICAST_DELEGATE_OneParam(FSubtitleDelegate, ULimenSubtitle* /* SubtitlePtr */);

/**
 * 
 */
UCLASS()
class LIMENSUBTITLES_API ULimenSubtitle : public ULimenWidget
{
	GENERATED_BODY()

public:
	FSubtitleDelegate OnSubtitleFinish;

	explicit ULimenSubtitle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void NativeConstruct() override;
	virtual void BeginDestroy() override;
	
	void SetSubtitleData(const FDataTableRowHandle& InSubtitleData);
	void StartDisplayingSubtitles();

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SubtitleCueSet(const FLimenSubtitleCue& InSubtitleCue);
	
	FDataTableRowHandle SubtitleData;

private:
	FTimerHandle CurrentCueTimerHandle;
	FTimerHandle NextCueTimerHandle;
	int32 SubtitleIndex;

	TArray<FLimenSubtitleCue*> SubtitleRows;
	void ShowCurrentSubtitle();
	void HideCurrentSubtitle();
};
