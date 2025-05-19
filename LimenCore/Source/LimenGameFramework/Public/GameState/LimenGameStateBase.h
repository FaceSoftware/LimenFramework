// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Network/LimenNetworkUtils.h"
#include "LimenGameStateBase.generated.h"

class ALimenPlayerStateBase;


USTRUCT()
struct FReplicatedPlayerArrayItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY()
	TWeakObjectPtr<APlayerState> PlayerState;

	FReplicatedPlayerArrayItem() = default;
	explicit FReplicatedPlayerArrayItem(APlayerState* InPlayerState);
	bool operator==(const FReplicatedPlayerArrayItem& Other) const;
	bool operator==(const APlayerState* Test) const;
};

USTRUCT()
struct FReplicatedPlayerArray : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FReplicatedPlayerArrayItem> Items;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo & DeltaParms);	
};
DECLARE_STRUCT_OPS_TYPE_TRAITS(FReplicatedPlayerArray)


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPlayerListChanged, const TArray<APlayerState*>&, PlayersList);

UCLASS()
class LIMENGAMEFRAMEWORK_API ALimenGameStateBase : public AGameStateBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPlayerListChanged OnPlayerListChanged;

	void AddPlayerToPlayerList(APlayerState* NewPlayer);
	void RemovePlayerFromPlayerList(APlayerState* Player);
	UFUNCTION(BlueprintCallable)
	const TArray<APlayerState*>& GetPlayerList() const;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PlayersListChanged();

private:
	UPROPERTY(ReplicatedUsing="OnRep_LimenPlayerList")
	FReplicatedPlayerArray ReplicatedPlayerList;
	UPROPERTY()
	TArray<APlayerState*> PlayerList;

	UFUNCTION()
	virtual void OnRep_LimenPlayerList();
};
