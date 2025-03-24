// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialoguePlayerBase.generated.h"


class UDataTable;

/**
 * 
 */
UCLASS(Abstract)
class LIMENSUBTITLES_API UDialoguePlayerBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void PlayDialogue(const UDataTable* InDialogueData) {}
};
