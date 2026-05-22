// Copyright Face Software. All Rights Reserved.


#include "PlayerState/LimenPlayerStateBase.h"

#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ALimenPlayerStateBase::ALimenPlayerStateBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void ALimenPlayerStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams Params;
	Params.Condition = ELifetimeCondition::COND_None;
	Params.RepNotifyCondition = REPNOTIFY_OnChanged;
	Params.bIsPushBased = true;
	
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, bIsClientReady, Params)
}

bool ALimenPlayerStateBase::IsLocalPlayerState() const
{
	const APlayerController* PC = GetPlayerController();
	return PC && PC->IsLocalController();
}

void ALimenPlayerStateBase::SetClientReady(const bool bIsReady)
{
	check(HasAuthority())
	if (bIsClientReady == bIsReady) { return; }
	
	bIsClientReady = bIsReady;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, bIsClientReady, this)
	
	OnClientReady.Broadcast(this);
}

bool ALimenPlayerStateBase::IsClientReady() const
{
	return bIsClientReady;
}

void ALimenPlayerStateBase::OnRep_ClientReady()
{
	OnClientReady.Broadcast(this);
}
