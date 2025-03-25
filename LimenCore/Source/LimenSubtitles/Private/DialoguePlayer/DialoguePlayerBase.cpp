// Fill out your copyright notice in the Description page of Project Settings.


#include "DialoguePlayer/DialoguePlayerBase.h"


UDialoguePlayerBase::UDialoguePlayerBase() : Super()
{
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
}

void UDialoguePlayerBase::FinishPlayDialogue()
{
	bHasFinished = true;
	OnDialogueFinished.Broadcast(this);
}
