// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BlueprintUtils.generated.h"


UENUM()
enum class EDynamicParameterType : uint8
{
	Float,
	Int,
	String,
};

/**
 * 
 */
USTRUCT(BlueprintType)
struct LIMENCORE_API FDynamicParameter
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FName Id;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FText DisplayName;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	EDynamicParameterType Type = EDynamicParameterType::String;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FString DefaultValue;
};
