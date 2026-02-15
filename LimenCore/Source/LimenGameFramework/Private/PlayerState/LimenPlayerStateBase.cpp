// Copyright Face Software. All Rights Reserved.


#include "PlayerState/LimenPlayerStateBase.h"

#include "GameFramework/PlayerController.h"


ALimenPlayerStateBase::ALimenPlayerStateBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool ALimenPlayerStateBase::IsLocalPlayerState() const
{
	const APlayerController* PC = GetPlayerController();
	return PC && PC->IsLocalController();
}
