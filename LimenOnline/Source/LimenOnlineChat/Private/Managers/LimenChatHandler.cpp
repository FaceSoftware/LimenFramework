// Fill out your copyright notice in the Description page of Project Settings.


#include "LimenOnlineChat/Public/Actors/LimenChatHandler.h"

#include "TimerManager.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"
#include "UMG/LimenChatBox.h"


ALimenChatHandler::ALimenChatHandler(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bReplicates = true;
	bAlwaysRelevant = true;
}

void ALimenChatHandler::BeginPlay()
{
	Super::BeginPlay();
}

void ALimenChatHandler::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALimenChatHandler, ChatMessages);
}

void ALimenChatHandler::RegisterMessage(const FChatMessage& InMessage)
{
	ChatMessages.Items.Push(InMessage);
	ChatMessages.MarkItemDirty(ChatMessages.Items.Last());

	if (GetNetMode() == NM_DedicatedServer)
	{
		OnNewChatMessageReceived.Broadcast(ChatMessages.Items.Last());
	}
}

TArray<FChatMessage> ALimenChatHandler::GetChatMessages() const
{
	return ChatMessages.Items;
}

void ALimenChatHandler::OnRep_ChatMessages()
{
	ChatMessages.Items.Sort([](const FChatMessage& A, const FChatMessage& B)
	{
		return A.Timestamp < B.Timestamp;
	});
	
	OnNewChatMessageReceived.Broadcast(ChatMessages.Items.Last());
}
