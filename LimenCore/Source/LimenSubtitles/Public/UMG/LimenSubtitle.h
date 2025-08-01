// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Structs/SubtitleCoreStructs.h"
#include "UObject/StrongObjectPtr.h"
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

	void SetSubtitleData(const UDataTable* InSubtitleData);
	void StartDisplayingSubtitles();
	void StopDisplayingSubtitles();

protected:
	TStrongObjectPtr<const UDataTable> SubtitleData;

	UFUNCTION(BlueprintImplementableEvent)
	void SubtitleCueSet(const FLimenDialogueCue& InSubtitleCue);
	virtual void ShowWidgetMethod() override;

private:
	FTimerHandle CurrentCueTimerHandle;
	FTimerHandle NextCueTimerHandle;
	int32 SubtitleIndex;

	TArray<FLimenDialogueCue*> SubtitleRows;
	void ShowCurrentSubtitle();
	void HideCurrentSubtitle();
};
