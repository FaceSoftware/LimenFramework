// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "LimenGameViewportClient.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FCharInputEvent, const TCHAR);
DECLARE_MULTICAST_DELEGATE_OneParam(FKeyInputEvent, const FInputKeyEventArgs&);

UCLASS()
class LIMENCORE_API ULimenGameViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	FCharInputEvent OnCharInput;
	FKeyInputEvent OnKeyInput;

	virtual bool InputKey(const FInputKeyEventArgs& EventArgs) override;
	virtual bool InputChar(FViewport* InViewport, int32 ControllerId, TCHAR Character) override;
};
