// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SubtitleCoreStructs.generated.h"

USTRUCT(BlueprintType)
struct FLimenDialogueCue : public FTableRowBase
{
	GENERATED_BODY()

public:
	FLimenDialogueCue() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EndTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SpeakerDisplayName = FText::FromString("Speaker");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor SpeakerTextColor = FColor::Cyan;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubtitleText = FText::FromString("SubtitleText");
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FColor SubtitleTextColor = FColor::White;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName SpeakerID;
};
