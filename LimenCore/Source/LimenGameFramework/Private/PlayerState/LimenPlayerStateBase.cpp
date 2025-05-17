// Copyright Face Software. All Rights Reserved.


#include "PlayerState/LimenPlayerStateBase.h"

#include <ThirdParty/Steamworks/Steamv157/sdk/public/steam/isteamutils.h>

#include "OnlineSubsystemUtils.h"
#include "TextureResource.h"
#include "Engine/Texture2D.h"
#include "GameFramework/PlayerController.h"
#include "ThirdParty/Steamworks/Steamv157/sdk/public/steam/isteamfriends.h"


ALimenPlayerStateBase::ALimenPlayerStateBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

bool ALimenPlayerStateBase::IsLocalPlayerState() const
{
	const APlayerController* PC = GetPlayerController();
	return PC && PC->IsLocalController();
}
