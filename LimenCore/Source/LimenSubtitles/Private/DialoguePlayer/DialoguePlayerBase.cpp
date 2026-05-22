// Copyright © 2024 FaceSoftware. All rights reserved.


#include "DialoguePlayer/DialoguePlayerBase.h"


UDialoguePlayerBase::UDialoguePlayerBase() : Super()
{
	PlayerId = FGuid::NewGuid();
}

void UDialoguePlayerBase::BeginDestroy()
{
	UObject::BeginDestroy();

	if (bHasFinished.IsSet())
	{
		checkf(bHasFinished, TEXT("Call UDialoguePlayerBase::FinishPlayDialogue so the subsystem knows when to cleanup dialogue players."))
	}
}

void UDialoguePlayerBase::PlayDialogue(const UDataTable* InDialogueData)
{
	bHasFinished = false;
	DialogueData = InDialogueData;
}

void UDialoguePlayerBase::FinishPlayDialogue()
{
	bHasFinished = true;
	OnDialogueFinished.Broadcast(this);
}

void UDialoguePlayerBase::StopDialogue()
{
}

const UDataTable* UDialoguePlayerBase::GetDialogueData() const
{
	return DialogueData.Get();
}

FGuid UDialoguePlayerBase::GetPlayerId() const
{
	return PlayerId;
}
