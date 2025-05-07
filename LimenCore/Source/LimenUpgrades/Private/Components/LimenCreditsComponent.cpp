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

void ULimenCreditsComponent::DepositCredits(const int32 NewCredits)
{
	check(NewCredits >= 0)
	CurrentCredits += NewCredits;
	CreditsUpdated();
}

bool ULimenCreditsComponent::WithdrawCredits(const int32 OutCredits)
{
	check(OutCredits >= 0)

	if (!bAllowNegativeBalance && CurrentCredits < OutCredits)
	{
		return false;
	}

	CurrentCredits -= OutCredits;
	CreditsUpdated();
	return true;
}

int32 ULimenCreditsComponent::GetCredits() const
{
	return CurrentCredits;
}

void ULimenCreditsComponent::SetStartingCredits(const int32 NewStartingCredits)
{
	StartingCredits = NewStartingCredits;
}
void ULimenCreditsComponent::CreditsUpdated()
{
	OnCreditsUpdated.Broadcast(this, CurrentCredits);
}

void ULimenCreditsComponent::OnRep_CurrentCredits()
{
	CreditsUpdated();
}
