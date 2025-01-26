// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenMenuWidget.h"
#include "LimenChatBox.generated.h"

struct FChatMessage;
class ALimenChatHandler;
/**
 * 
 */
UCLASS()
class LIMENONLINECHAT_API ULimenChatBox : public ULimenMenuWidget
{
	GENERATED_BODY()

public:
	void BindChatHandler(ALimenChatHandler* Handler);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void MessageReceived(const FChatMessage& NewMessage);
};
