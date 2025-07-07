// Copyright © 2024 FaceSoftware. All rights reserved.


#include "UMG/LimenChatBox.h"

#include "Actors/LimenChatHandler.h"


void ULimenChatBox::BindChatHandler(ALimenChatHandler* Handler)
{
	Handler->OnNewChatMessageReceived.AddLambda([this] (const FChatMessage& NewMessage)
	{
		MessageReceived(NewMessage);
	});

	TArray<FChatMessage> Messages = Handler->GetChatMessages();
	if (Messages.IsEmpty())
	{
		return;
	}

	for (auto& Message : Messages)
	{
		MessageReceived(Message);
	}
}
