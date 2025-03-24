// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DialoguePlayerBase.generated.h"


class UDataTable;
class UDialoguePlayerBase;

DECLARE_MULTICAST_DELEGATE_OneParam(FDialogueEvent, UDialoguePlayerBase* /* DialoguePlayer */);

/**
 * 
 */
UCLASS(Abstract)
class LIMENSUBTITLES_API UDialoguePlayerBase : public UObject
{
	GENERATED_BODY()

public:
	UDialoguePlayerBase();
	virtual void BeginDestroy() override;

	virtual void PlayDialogue(const UDataTable* InDialogueData);
	virtual void FinishPlayDialogue();

	template<typename UserClass>
	FORCEINLINE void SubscribeToFinishEvent(UserClass* Object, typename TMemFunPtrType<false, UserClass, void(UDialoguePlayerBase*)>::Type InFunc)
	{
		OnDialogueFinished.AddUObject(Object, InFunc);
	}

private:
	FDialogueEvent OnDialogueFinished;
	TOptional<bool> bHasFinished;
};
