// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "SubtitleCoreStructs.generated.h"

USTRUCT(BlueprintType)
struct FLimenSubtitleCue : public FTableRowBase
{
	GENERATED_BODY()

public:
	FLimenSubtitleCue() = default;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StartTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float EndTime = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText SubtitleText;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FText Speaker;
};
