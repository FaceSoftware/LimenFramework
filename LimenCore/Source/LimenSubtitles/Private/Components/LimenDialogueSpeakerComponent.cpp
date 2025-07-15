// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenDialogueSpeakerComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"


ULimenDialogueSpeakerComponent::ULimenDialogueSpeakerComponent(): bIsRegisteredWithSubsystem(false)
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void ULimenDialogueSpeakerComponent::BeginPlay()
{
	Super::BeginPlay();

	RegisterSpeaker();
}

void ULimenDialogueSpeakerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	

	Super::EndPlay(EndPlayReason);
}

UActorComponent* ULimenDialogueSpeakerComponent::GetDialogueAudioComponent() const
{
	if (DialogueAudioComponent.IsValid()) return DialogueAudioComponent.Get();

	const AActor* Owner = GetOwner();
	if (!Owner) return nullptr;

	const TArray<UActorComponent*> Components = Owner->GetComponentsByTag(UActorComponent::StaticClass(), DialogueAudioComponentTag);
	if (Components.IsEmpty()) return nullptr;

	if (!ensureAlwaysMsgf(Components.Num() == 1, TEXT("Unable to find dialogue audio component"))) return nullptr;

	DialogueAudioComponent = Components[0];
	return DialogueAudioComponent.Get();
}

void ULimenDialogueSpeakerComponent::SetSpeakerID(const FName& NewSpeakerID)
{
	check(!bIsRegisteredWithSubsystem);
	SpeakerID = NewSpeakerID;
}

const FName& ULimenDialogueSpeakerComponent::GetSpeakerID() const
{
	return SpeakerID;
}

void ULimenDialogueSpeakerComponent::PlayDialogue(const UDataTable* InDialogueData,
												  const FDialogueEndEvent OnDialogueEnd)
{
	auto* DialogueSubsystem = GetWorld()->GetSubsystem<ULimenDialogueSubsystem>();
	if (!DialogueSubsystem) return;

	DialogueSubsystem->BP_PlayDialogue(InDialogueData, OnDialogueEnd);
}

void ULimenDialogueSpeakerComponent::RegisterSpeaker()
{
	if (SpeakerID.IsNone()) return;

	auto* DialogueSubsystem = GetWorld()->GetSubsystem<ULimenDialogueSubsystem>();
	if (!DialogueSubsystem) return;

	bIsRegisteredWithSubsystem = DialogueSubsystem->RegisterSpeaker(SpeakerID, GetDialogueAudioComponent());
}

void ULimenDialogueSpeakerComponent::UnRegisterSpeaker()
{
	if (SpeakerID.IsNone()) return;

	auto* DialogueSubsystem = GetWorld()->GetSubsystem<ULimenDialogueSubsystem>();
	if (!DialogueSubsystem) return;

	DialogueSubsystem->UnregisterSpeaker(SpeakerID);
	bIsRegisteredWithSubsystem = false;
}
