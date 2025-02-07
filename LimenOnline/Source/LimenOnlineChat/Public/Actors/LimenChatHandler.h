// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "Actors/LimenGameplayManager.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "LimenChatHandler.generated.h"


class ULimenChatBox;

USTRUCT(Blueprintable)
struct FChatMessage : public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_OneParam(FReplicationEvent, const FFastArraySerializer& /* Serializer */ )

	FChatMessage() = default;
	explicit FChatMessage(const FString& InSenderName, const FString& InMessageContent)
	{
		SenderName = InSenderName;
		MessageContent = InMessageContent;
		Timestamp = FDateTime::Now();
		Id = FGuid::NewGuid();
	}

	FReplicationEvent OnPreReplicatedRemove;
	FReplicationEvent OnPostReplicatedAdd;
	FReplicationEvent OnPostReplicatedChange;
	
	UPROPERTY(BlueprintReadOnly)
	FString SenderName;

	UPROPERTY(BlueprintReadOnly)
	FString MessageContent;
	
	UPROPERTY(BlueprintReadOnly)
	FDateTime Timestamp;
	
	UPROPERTY(BlueprintReadOnly)
	FGuid Id;

	struct FMessageArrayContainer;

	void PreReplicatedRemove(const struct FFastArraySerializer& InArraySerializer)
	{
		const bool bResult = OnPreReplicatedRemove.ExecuteIfBound(InArraySerializer);
	}
	void PostReplicatedAdd(const FFastArraySerializer& InArraySerializer)
	{
		const bool bResult = OnPostReplicatedAdd.ExecuteIfBound(InArraySerializer);
	}
	void PostReplicatedChange(const FFastArraySerializer& InArraySerializer)
	{
		const bool bResult = OnPostReplicatedChange.ExecuteIfBound(InArraySerializer);
	}
};

USTRUCT()
struct FMessageArrayContainer : public FFastArraySerializer
{
	GENERATED_BODY()
	
	UPROPERTY()
	TArray<FChatMessage> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FChatMessage, FMessageArrayContainer>(Items, DeltaParams, *this);
	}
};

template<>
struct TStructOpsTypeTraits<FMessageArrayContainer> : public TStructOpsTypeTraitsBase2<FMessageArrayContainer>
{
	enum 
	{
		WithNetDeltaSerializer = true,
   };
};


DECLARE_MULTICAST_DELEGATE_OneParam(FChatMessageDelegate, FChatMessage& /* Message */)

UCLASS()
class LIMENONLINECHAT_API ALimenChatHandler : public ALimenGameplayManager
{
	GENERATED_BODY()

	friend struct FChatMessage;

public:
	FChatMessageDelegate OnNewChatMessageReceived;
	
	explicit ALimenChatHandler(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual void RegisterMessage(const FChatMessage& InMessage);
	TArray<FChatMessage> GetChatMessages() const;

protected:

private:
	UPROPERTY(ReplicatedUsing=OnRep_ChatMessages)
	FMessageArrayContainer ChatMessages;

	UFUNCTION()
	void OnRep_ChatMessages();
};
