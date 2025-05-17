// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenCreditsComponent.h"

#include "Net/UnrealNetwork.h"


ULimenCreditsComponent::ULimenCreditsComponent()
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	
	StartingCredits = 0;
	CurrentCredits = 0;
	bAllowNegativeBalance = false;
}

void ULimenCreditsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentCredits,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

void ULimenCreditsComponent::BeginPlay()
{
	CurrentCredits = StartingCredits;

	Super::BeginPlay();
}

void ULimenCreditsComponent::DepositCredits(const int64 NewCredits)
{
	CurrentCredits += static_cast<uint64>(NewCredits);
	CreditsUpdated();
}

bool ULimenCreditsComponent::WithdrawCredits(const int64 OutCredits)
{
	if (!bAllowNegativeBalance && CurrentCredits < static_cast<uint64>(OutCredits))
	{
		return false;
	}

	CurrentCredits -= OutCredits;
	CreditsUpdated();
	return true;
}

FString ULimenCreditsComponent::GetCreditsString() const
{
	return FString::Printf(TEXT("%llu"), CurrentCredits);
}

int64 ULimenCreditsComponent::GetCredits() const
{
	return CurrentCredits;
}

void ULimenCreditsComponent::SetStartingCredits(const int32 NewStartingCredits)
{
	StartingCredits = NewStartingCredits;
}
void ULimenCreditsComponent::CreditsUpdated()
{
	OnCreditsUpdated.Broadcast(this, GetCreditsString());
}

void ULimenCreditsComponent::OnRep_CurrentCredits()
{
	CreditsUpdated();
}
