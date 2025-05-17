// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include <steam/isteammatchmaking.h>
#include <steam/steam_api_common.h>
#include "CoreMinimal.h"
#include "Interfaces/LimenOnlineManagerInterface.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "LimenSteamOnlineManager.generated.h"

class FOnlineSessionSearchResult;
enum ELobbyType;


UCLASS()
class LIMENONLINEMANAGER_API ULimenSteamOnlineManager : public UGameInstanceSubsystem, public ILimenOnlineManagerInterface
{
	GENERATED_BODY()

public:
	static ELimenLobbyType FromSteamLobbyType(ELobbyType Type);
	static ELobbyType FromLimenLobbyType(ELimenLobbyType Type);


	UPROPERTY(BlueprintAssignable)
	FLimenLobbyDelegate OnLobbyCreated;
	UPROPERTY(BlueprintAssignable)
	FLimenLobbyDelegate OnLobbyJoined;
	UPROPERTY(BlueprintAssignable)
	FLimenLobbyDelegate OnLobbyGameServerSet;
	UPROPERTY(BlueprintAssignable)
	FLimenLobbyDelegate OnLobbyMemberUpdated;
	UPROPERTY(BlueprintAssignable)
	FLimenLobbyDelegate OnMemberDataUpdated;
	UPROPERTY(BlueprintAssignable)
	FLimenLobbyDelegate OnMemberAvatarLoaded;

	ULimenSteamOnlineManager();
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "Online Manager|Steam")
	virtual void SetLobbyGameServerP2P();
	UFUNCTION(BlueprintCallable, Category = "Online Manager|Steam")
	virtual void ConnectToGameServer();
	UFUNCTION(BlueprintCallable, Category = "Online Manager|Steam")
	virtual void CreateLobby(ELimenLobbyType Type, int32 MaxMembers) override;
	UFUNCTION(BlueprintCallable, Category = "Online Manager|Steam")
	virtual void LeaveLobby() override;
	UFUNCTION(BlueprintCallable, Category = "Online Manager|Steam")
	virtual bool IsInLobby() override;
	UFUNCTION(BlueprintCallable, Category = "Online Manager|Steam")
	virtual bool IsLobbyHost();
	UFUNCTION(BlueprintCallable, Category = "Online Manager|Steam")
	virtual TArray<FLimenOnlineUser> GetLobbyParticipants() override;

private:
	inline static const FName SessionIDKey = TEXT("SessionSteamID");

	CSteamID CurrentLobbyID;
	CSteamID CurrentLobbyGameServerID;
	TArray<CSteamID> CurrentLobbyMembersID;
	bool bIsLobbyHost;

	FCriticalSection CallbackDataSection;
	STEAM_CALLBACK(ThisClass, LobbyCreated, LobbyCreated_t);
	STEAM_CALLBACK(ThisClass, LobbyJoined, LobbyEnter_t);
	STEAM_CALLBACK(ThisClass, LobbyInviteAccepted, GameLobbyJoinRequested_t);
	STEAM_CALLBACK(ThisClass, LobbyMemberUpdated, LobbyChatUpdate_t);
	STEAM_CALLBACK(ThisClass, MemberDataUpdated, PersonaStateChange_t);
	STEAM_CALLBACK(ThisClass, FriendAvatarImageLoaded, AvatarImageLoaded_t);
	STEAM_CALLBACK(ThisClass, LobbyGameServerSet, LobbyGameCreated_t);
	
	static UTexture2D* CreateTextureFromRGBA(const uint8* ImageData, int32 Width, int32 Height);
};
