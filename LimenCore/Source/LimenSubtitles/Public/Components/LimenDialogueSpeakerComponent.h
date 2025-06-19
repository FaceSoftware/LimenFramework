// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenDialogueSpeakerComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENSUBTITLES_API ULimenDialogueSpeakerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenDialogueSpeakerComponent();
	virtual void BeginPlay() override;
	

	UActorComponent* GetDialogueAudioComponent() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SpeakerID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DialogueAudioComponentTag;

	virtual void RegisterSpeaker();

private:
	mutable TWeakObjectPtr<UActorComponent> DialogueAudioComponent;
};
