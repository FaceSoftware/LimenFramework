// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Subsystems/LimenDialogueSubsystem.h"
#include "LimenDialogueSpeakerComponent.generated.h"


class FDialogueEndEvent;
class UDataTable;

/**
 * @brief Represents an entity (identified by its ID) that emits dialogue,
 * such as a radio or device—not necessarily a character.
 * The actual dialogue participants are defined in the dialogue DataTable.
 * For example, if a radio plays a conversation, the radio is the speaker, not the people speaking in the dialogue.
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENSUBTITLES_API ULimenDialogueSpeakerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenDialogueSpeakerComponent();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UActorComponent* GetDialogueAudioComponent() const;

	void SetSpeakerID(const FName& NewSpeakerID);
	const FName& GetSpeakerID() const;

	UFUNCTION(BlueprintCallable)
	void PlayDialogue(const UDataTable* InDialogueData, const FDialogueEndEvent OnDialogueEnd);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName SpeakerID;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName DialogueAudioComponentTag;

	virtual void RegisterSpeaker();
	virtual void UnRegisterSpeaker();

private:
	mutable TWeakObjectPtr<UActorComponent> DialogueAudioComponent;
	bool bIsRegisteredWithSubsystem;
};
