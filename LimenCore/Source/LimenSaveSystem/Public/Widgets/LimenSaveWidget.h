// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenWidget.h"
#include "LimenSaveWidget.generated.h"

/**
 * 
 */
UCLASS()
class LIMENSAVESYSTEM_API ULimenSaveWidget : public ULimenWidget
{
	GENERATED_BODY()

	friend class ULimenSaveSubsystem;

public:

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void SaveStarted();
	UFUNCTION(BlueprintImplementableEvent)
	void SaveFinished();

private:
};
