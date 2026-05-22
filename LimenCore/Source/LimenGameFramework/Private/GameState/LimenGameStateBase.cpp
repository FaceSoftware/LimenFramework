// Copyright Face Software. All Rights Reserved.


#include "GameState/LimenGameStateBase.h"

#include "Net/UnrealNetwork.h"
#include "PlayerState/LimenPlayerStateBase.h"


FReplicatedPlayerArrayItem::FReplicatedPlayerArrayItem(APlayerState* InPlayerState)
{
	PlayerState = InPlayerState;
}

bool FReplicatedPlayerArrayItem::operator==(const FReplicatedPlayerArrayItem& Other) const
{
	return Other.PlayerState == PlayerState;
}

bool FReplicatedPlayerArrayItem::operator==(const APlayerState* Test) const
{
	return PlayerState.Get() == Test;
}

bool FReplicatedPlayerArray::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
{
	return FastArrayDeltaSerialize<FReplicatedPlayerArrayItem, FReplicatedPlayerArray>(Items, DeltaParms, *this);
}

void ALimenGameStateBase::AddPlayerToPlayerList(APlayerState* NewPlayer)
{
	if (ReplicatedPlayerList.Items.Contains(FReplicatedPlayerArrayItem(NewPlayer))) return;
	
	FReplicatedPlayerArrayItem Item;
	Item.PlayerState = NewPlayer;

	const int32 Index = ReplicatedPlayerList.Items.Add(Item);
	ReplicatedPlayerList.MarkItemDirty(ReplicatedPlayerList.Items[Index]);
	PlayersListChanged();
}

void ALimenGameStateBase::RemovePlayerFromPlayerList(APlayerState* Player)
{
	const int32 Index = ReplicatedPlayerList.Items.IndexOfByPredicate(
		[this, &Player] (const FReplicatedPlayerArrayItem& Item)
		{
			return Item.PlayerState == Player;
		});

	if (Index == INDEX_NONE) return;

	ReplicatedPlayerList.Items.RemoveAt(Index);
	ReplicatedPlayerList.MarkArrayDirty();
}

const TArray<APlayerState*>& ALimenGameStateBase::GetPlayerList() const
{
	return PlayerList;
}

void ALimenGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, ReplicatedPlayerList,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_Always, true))
}

void ALimenGameStateBase::PlayersListChanged()
{
	PlayerList.Empty(ReplicatedPlayerList.Items.Num());
	for (auto& ListItem : ReplicatedPlayerList.Items)
	{
		PlayerList.Push(ListItem.PlayerState.Get());
	}
	
	OnPlayerListChanged.Broadcast(PlayerList);
}

void ALimenGameStateBase::OnRep_LimenPlayerList()
{
	PlayersListChanged();
}
