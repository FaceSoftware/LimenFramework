// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenDialogueSpeakerComponent.h"

#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Subsystems/LimenDialogueSubsystem.h"


ULimenDialogueSpeakerComponent::ULimenDialogueSpeakerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bAutoActivate = true;
}

void ULimenDialogueSpeakerComponent::BeginPlay()
{
	Super::BeginPlay();

	RegisterSpeaker();
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

void ULimenDialogueSpeakerComponent::RegisterSpeaker()
{
	if (SpeakerID.IsNone()) return;

	auto* DialogueSubsystem = GetWorld()->GetSubsystem<ULimenDialogueSubsystem>();
	if (!DialogueSubsystem) return;

	DialogueSubsystem->RegisterSpeaker(SpeakerID, GetDialogueAudioComponent());
}
