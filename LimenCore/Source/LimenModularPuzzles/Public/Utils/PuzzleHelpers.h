// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "PuzzleHelpers.generated.h"


UENUM()
enum class EPuzzleOp : uint8
{
	And,
	Or,
	Xor,
	AtLeast,
	TimerGate,
};

USTRUCT(BlueprintType)
struct LIMENMODULARPUZZLES_API FPuzzleConditionNode
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere)
	FName NodeId;
	UPROPERTY(EditAnywhere)
	TArray<FName> InputIDs;

	UPROPERTY(EditAnywhere, Category="Node Logic")
	EPuzzleOp Operator = EPuzzleOp::And;
	UPROPERTY(EditAnywhere, Category="Node Logic", meta=(EditCondition="Operator == EPuzzleOp::AtLeast", EditConditionHides="true"))
	int32 Threshold = 0;
	UPROPERTY(EditAnywhere, Category="Node Logic", meta=(EditCondition="Operator==EPuzzleOp::TimerGate", EditConditionHides="true", ClampMin="0.0"))
	float Duration = 1.0f;                                   // Seconds inputs must stay true
	UPROPERTY(EditAnywhere, Category="Node Logic", meta=(EditCondition="Operator==EPuzzleOp::TimerGate", EditConditionHides="true"))
	bool bLatch = false;                                     // Stay true once fired until reset
	UPROPERTY(EditAnywhere, Category="Node Logic", meta=(EditCondition="Operator==EPuzzleOp::TimerGate", ClampMin="0.0", EditConditionHides="true"))
	float Cooldown = 0.0f; 
};

struct FTimerGateState
{
	float TrueStartTime = -1.f;  // When inputs first became true
	float LastOutputTime = -FLT_MAX;
	bool  bLatched = false;
};
