// Copyright FaceSoftware. All Rights Reserved.


#include "OnlineManagers/LimenSteamOnlineManager.h"

#include <steam/isteamuser.h>
#include <steam/steam_api.h>
#include "TextureResource.h"
#include "TimerManager.h"
#include "Async/Async.h"
#include "Engine/NetConnection.h"
#include "Engine/Texture2D.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "steam/isteammatchmaking.h"


ELimenLobbyType ULimenSteamOnlineManager::FromSteamLobbyType(const ELobbyType Type)
{
	switch (Type)
	{
	case k_ELobbyTypePrivate:
		return ELimenLobbyType::Private;

	case k_ELobbyTypeFriendsOnly:
		return ELimenLobbyType::FriendsOnly;

	case k_ELobbyTypePublic:
		return ELimenLobbyType::Public;

	case k_ELobbyTypeInvisible:
		return ELimenLobbyType::Invisible;

	default:
		break;
	}

	return ELimenLobbyType::Unsupported;
}

ELobbyType ULimenSteamOnlineManager::FromLimenLobbyType(const ELimenLobbyType Type)
{
	check(Type != ELimenLobbyType::Unsupported);

	switch (Type)
	{
	case ELimenLobbyType::Private:
		return ELobbyType::k_ELobbyTypePrivate;

	case ELimenLobbyType::FriendsOnly:
		return ELobbyType::k_ELobbyTypeFriendsOnly;

	case ELimenLobbyType::Invisible:
		return ELobbyType::k_ELobbyTypeInvisible;

	case ELimenLobbyType::Public:
		return ELobbyType::k_ELobbyTypePublic;

	default:
		break;
	}

	return ELobbyType::k_ELobbyTypePrivate;
}

ULimenSteamOnlineManager::ULimenSteamOnlineManager() : Super()
{
	CurrentLobbyID.Clear();
	bIsLobbyHost = false;
}

bool ULimenSteamOnlineManager::ShouldCreateSubsystem(UObject* Outer) const
{
	if (!Super::ShouldCreateSubsystem(Outer))
	{
		return false;
	}
	
	// return SteamAPI_Init();
	return false;
}

void ULimenSteamOnlineManager::Initialize(FSubsystemCollectionBase& Collection)
{	
	Super::Initialize(Collection);

	LobbyCreatedCallback.Register(this, &ThisClass::LobbyCreated);
	LobbyEnterCallback.Register(this, &ThisClass::LobbyJoined);
	LobbyInviteAcceptedCallback.Register(this, &ThisClass::LobbyInviteAccepted);
	LobbyMemberUpdatedCallback.Register(this, &ThisClass::LobbyMemberUpdated);
	MemberDataUpdatedCallback.Register(this, &ThisClass::MemberDataUpdated);
	FriendAvatarImageLoadedCallback.Register(this, &ThisClass::FriendAvatarImageLoaded);
	LobbyGameServerSetCallback.Register(this, &ThisClass::LobbyGameServerSet);	
}

void ULimenSteamOnlineManager::Deinitialize()
{
	SteamAPI_Shutdown();
	Super::Deinitialize();
}

void ULimenSteamOnlineManager::SetLobbyGameServerP2P()
{
	FScopeLock Lock(&CallbackDataSection);

	ISteamMatchmaking* MatchmakingInterface = SteamMatchmaking();
	if (!MatchmakingInterface) return;

	ISteamUser* UserInterface = SteamUser();
	if (!UserInterface) return;

	check(CurrentLobbyID.IsValid())
	MatchmakingInterface->SetLobbyGameServer(CurrentLobbyID, 0, 0, UserInterface->GetSteamID());
}

void ULimenSteamOnlineManager::ConnectToGameServer()
{
	FScopeLock Lock(&CallbackDataSection);

	check(CurrentLobbyID.IsValid());
	check(CurrentLobbyGameServerID.IsValid());

	if (!GetWorld() || !GetWorld()->GetFirstPlayerController()) return;

	const FString SteamURL = FString::Printf(TEXT("Steam.%llu"), CurrentLobbyGameServerID.ConvertToUint64());
	APlayerController* PC = GetWorld()->GetFirstPlayerController();

	// Delay travel until net connection is ready
	FTimerDelegate TravelDelegate;
	TravelDelegate.BindLambda([PC, SteamURL, TravelDelegate]
	{
		if (PC && PC->GetNetConnection() && PC->GetNetConnection()->GetConnectionState() == USOCK_Open)
		{
			PC->ClientTravel(SteamURL, TRAVEL_Absolute);
		}
		else
		{
			// Retry in 0.5s until connection becomes valid
			PC->GetWorldTimerManager().SetTimerForNextTick(TravelDelegate); // Optional: or add delay
		}
	});

	// Start first check
	PC->GetWorldTimerManager().SetTimerForNextTick(TravelDelegate);
}

void ULimenSteamOnlineManager::CreateLobby(const ELimenLobbyType Type, const int32 MaxMembers)
{
	FScopeLock Lock(&CallbackDataSection);

	ISteamMatchmaking* MatchmakingInterface = SteamMatchmaking();
	if (!MatchmakingInterface) return;

	MatchmakingInterface->CreateLobby(FromLimenLobbyType(Type), MaxMembers);
}

void ULimenSteamOnlineManager::LeaveLobby()
{
	FScopeLock Lock(&CallbackDataSection);

	ISteamMatchmaking* MatchmakingInterface = SteamMatchmaking();
	if (!MatchmakingInterface) return;

	MatchmakingInterface->LeaveLobby(CurrentLobbyID);
	CurrentLobbyID.Clear();
	CurrentLobbyMembersID.Empty();
	bIsLobbyHost = false;
}

bool ULimenSteamOnlineManager::IsInLobby()
{
	FScopeLock Lock(&CallbackDataSection);

	return CSteamID(CurrentLobbyID).IsValid();
}

bool ULimenSteamOnlineManager::IsLobbyHost()
{
	return bIsLobbyHost;
}

TArray<FLimenOnlineUser> ULimenSteamOnlineManager::GetLobbyParticipants()
{
	FScopeLock Lock(&CallbackDataSection);

	ISteamFriends* FriendsInterface = SteamFriends();
	if (!FriendsInterface) return TArray<FLimenOnlineUser>();

	ISteamUtils* Utils = SteamUtils();
	if (!Utils) return TArray<FLimenOnlineUser>();
	
	TArray<FLimenOnlineUser> LobbyMembers;

	for (const CSteamID& Member : CurrentLobbyMembersID)
	{
		FLimenOnlineUser Temp;
		Temp.Nickname = FriendsInterface->GetFriendPersonaName(Member);

//		const int32 AvatarHandle = FriendsInterface->GetLargeFriendAvatar(Member);
//		uint8* Image = new uint8[32 * 32 * 4 * sizeof(char)]; // Width * Height * 4 bytes per pixel (RGBA)
//
//		if (Utils->GetImageRGBA(AvatarHandle, Image, 32 * 32 * 4 * sizeof(char)))
//		{
//			Temp.Avatar = CreateTextureFromRGBA(Image, 32, 32);
//		}

		LobbyMembers.Push(Temp);
	}

	return LobbyMembers;
}

void ULimenSteamOnlineManager::LobbyCreated(LobbyCreated_t* pParam)
{
	FScopeLock Lock(&CallbackDataSection);

	if (pParam->m_eResult != EResult::k_EResultOK)
	{
		// Failed to create
		return;
	}

	ISteamMatchmaking* MatchmakingInterface = SteamMatchmaking();
	if (!MatchmakingInterface) return;

	ISteamUser* UserInterface = SteamUser();
	if (!UserInterface) return;

	const CSteamID LobbyId = pParam->m_ulSteamIDLobby;
	const int32 MaxMembers = MatchmakingInterface->GetLobbyMemberLimit(LobbyId);
	bIsLobbyHost = true;

	AsyncTask(ENamedThreads::GameThread, [this, MaxMembers, MatchmakingInterface, UserInterface]
	{
		MatchmakingInterface->SetLobbyOwner(CurrentLobbyID, UserInterface->GetSteamID());
		OnLobbyCreated.Broadcast();
	});
}

void ULimenSteamOnlineManager::LobbyJoined(LobbyEnter_t* pParam)
{
	FScopeLock Lock(&CallbackDataSection);

	if (pParam->m_EChatRoomEnterResponse != k_EChatRoomEnterResponseSuccess)
	{
		// Failed to join
		return;
	}

	CurrentLobbyID = CSteamID(pParam->m_ulSteamIDLobby);
	check(CurrentLobbyID.IsValid())

	ISteamMatchmaking* MatchmakingInterface = SteamMatchmaking();
	if (!MatchmakingInterface) return;

	const int32 LobbyMembers = MatchmakingInterface->GetNumLobbyMembers(CurrentLobbyID);
	for (int i = 0; i < LobbyMembers; ++i)
	{
		const CSteamID Member = MatchmakingInterface->GetLobbyMemberByIndex(CurrentLobbyID, i);
		CurrentLobbyMembersID.AddUnique(Member);
	}

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		OnLobbyJoined.Broadcast();
	});
}

void ULimenSteamOnlineManager::LobbyInviteAccepted(GameLobbyJoinRequested_t* pParam)
{
	FScopeLock Lock(&CallbackDataSection);

	if (!pParam->m_steamIDLobby.IsValid()) return;

	uint64 LobbyID = pParam->m_steamIDLobby.ConvertToUint64();

	AsyncTask(ENamedThreads::GameThread, [this, LobbyID]
	{
		ISteamMatchmaking* MatchmakingInterface = SteamMatchmaking();
		if (!MatchmakingInterface) return;

		MatchmakingInterface->JoinLobby(CSteamID(LobbyID));
	});
}
	

void ULimenSteamOnlineManager::LobbyMemberUpdated(LobbyChatUpdate_t* pParam)
{
	FScopeLock Lock(&CallbackDataSection);

	if (pParam->m_rgfChatMemberStateChange & k_EChatMemberStateChangeEntered)
	{
		// New user joined
		CurrentLobbyMembersID.AddUnique(pParam->m_ulSteamIDUserChanged);
	}
	if (pParam->m_rgfChatMemberStateChange & k_EChatMemberStateChangeLeft)
	{
		// User left
		CurrentLobbyMembersID.Remove(pParam->m_ulSteamIDUserChanged);
	}
	if (pParam->m_rgfChatMemberStateChange & k_EChatMemberStateChangeDisconnected)
	{
		// User disconnected
		CurrentLobbyMembersID.Remove(pParam->m_ulSteamIDUserChanged);
	}
	if (pParam->m_rgfChatMemberStateChange & k_EChatMemberStateChangeKicked)
	{
		// User kicked
		CurrentLobbyMembersID.Remove(pParam->m_ulSteamIDUserChanged);
	}
	if (pParam->m_rgfChatMemberStateChange & k_EChatMemberStateChangeBanned)
	{
		// User banned
		CurrentLobbyMembersID.Remove(pParam->m_ulSteamIDUserChanged);
	}

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		OnLobbyMemberUpdated.Broadcast();
	});
}

void ULimenSteamOnlineManager::MemberDataUpdated(PersonaStateChange_t* pParam)
{
	FScopeLock Lock(&CallbackDataSection);

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		OnMemberDataUpdated.Broadcast();
	});
}

void ULimenSteamOnlineManager::FriendAvatarImageLoaded(AvatarImageLoaded_t* pParam)
{
	FScopeLock Lock(&CallbackDataSection);

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		OnMemberAvatarLoaded.Broadcast();
	});
}

void ULimenSteamOnlineManager::LobbyGameServerSet(LobbyGameCreated_t* pParam)
{
	FScopeLock Lock(&CallbackDataSection);

	CurrentLobbyGameServerID = CSteamID(pParam->m_ulSteamIDGameServer);

	AsyncTask(ENamedThreads::GameThread, [this]
	{
		OnLobbyGameServerSet.Broadcast();
	});
}

UTexture2D* ULimenSteamOnlineManager::CreateTextureFromRGBA(const uint8* ImageData, int32 Width, int32 Height)
{
	UTexture2D* Texture = UTexture2D::CreateTransient(Width, Height, PF_R8G8B8A8);
	if (!Texture) return nullptr;

	// Lock the texture for writing
	FTexture2DMipMap& Mip = Texture->GetPlatformData()->Mips[0];
	void* Data = Mip.BulkData.Lock(LOCK_READ_WRITE);
	FMemory::Memcpy(Data, ImageData, Width * Height * 4); // 4 bytes per pixel (RGBA)
	Mip.BulkData.Unlock();

	// Update the texture on the rendering thread
	Texture->UpdateResource();

	return Texture;
}
