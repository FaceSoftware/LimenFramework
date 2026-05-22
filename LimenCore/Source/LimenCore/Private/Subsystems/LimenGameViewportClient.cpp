// Copyright FaceSoftware. All rights reserved.


#include "Subsystems/LimenGameViewportClient.h"


bool ULimenGameViewportClient::InputKey(const FInputKeyEventArgs& EventArgs)
{
	const bool bParentResult = Super::InputKey(EventArgs);

	OnKeyInput.Broadcast(EventArgs);
	return bParentResult;
}

bool ULimenGameViewportClient::InputChar(FViewport* InViewport, const int32 ControllerId, const TCHAR Character)
{
	const bool bParentResult = Super::InputChar(Viewport, ControllerId, Character);
	
	OnCharInput.Broadcast(Character);
	return bParentResult;
}
