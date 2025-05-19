// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenOnlineManagerInterface.generated.h"

class UTexture2D;


USTRUCT(BlueprintType)
struct FLimenOnlineUser
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FString Nickname;
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UTexture2D> Avatar;
};

UENUM()
enum class ELimenLobbyType : uint8
{
	Unsupported,
	Private,
	FriendsOnly,
	Public,
	Invisible,
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FLimenLobbyDelegate);

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class ULimenOnlineManagerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENONLINEMANAGER_API ILimenOnlineManagerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void CreateLobby(ELimenLobbyType Type, int32 MaxMembers) = 0;
	UFUNCTION(BlueprintCallable)
	virtual void LeaveLobby() = 0;
	UFUNCTION(BlueprintCallable)
	virtual bool IsInLobby() = 0;
	UFUNCTION(BlueprintCallable)
	virtual TArray<FLimenOnlineUser> GetLobbyParticipants() = 0;
};
