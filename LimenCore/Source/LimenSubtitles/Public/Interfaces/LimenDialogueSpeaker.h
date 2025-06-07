// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenDialogueSpeaker.generated.h"

// This class does not need to be modified.
UINTERFACE()
class ULimenDialogueSpeaker : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENSUBTITLES_API ILimenDialogueSpeaker
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	USceneComponent* GetDialogueAudioComponent() const;
	virtual USceneComponent* GetDialogueAudioComponent_Implementation() const = 0;
	UFUNCTION(BlueprintNativeEvent)
	FName GetSpeakerID() const;
	virtual FName GetSpeakerID_Implementation() const = 0;
};
