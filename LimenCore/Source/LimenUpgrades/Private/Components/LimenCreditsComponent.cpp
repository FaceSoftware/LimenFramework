// Copyright FaceSoftware. All Rights Reserved.


#include "Components/LimenCreditsComponent.h"

#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"


ULimenCreditsComponent::ULimenCreditsComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	SetIsReplicated(true);
	
	StartingCredits = 0;
	CurrentCredits = 0;
	bAllowNegativeBalance = false;
	Multiplier = 1.f;
}

void ULimenCreditsComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentCredits,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

void ULimenCreditsComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentCredits = StartingCredits;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentCredits, this)
}

void ULimenCreditsComponent::DepositCredits(const int64 NewCredits)
{
	check(GetOwner() && GetOwner()->HasAuthority())

	const int64 FlooredCredits = FMath::FloorToInt64(static_cast<double>(Multiplier) * static_cast<double>(NewCredits));
	CurrentCredits += static_cast<uint64>(FlooredCredits);
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentCredits, this)

	CreditsUpdated();
}

bool ULimenCreditsComponent::WithdrawCredits(const int64 OutCredits)
{
	check(GetOwner() && GetOwner()->HasAuthority())

	if (!bAllowNegativeBalance && CurrentCredits < static_cast<uint64>(OutCredits))
	{
		return false;
	}

	CurrentCredits -= OutCredits;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, CurrentCredits, this)
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
	check(GetOwner() && GetOwner()->HasAuthority())
	StartingCredits = NewStartingCredits;
}

void ULimenCreditsComponent::SetCreditsMultiplier(const float NewMultiplier)
{
	check(GetOwner() && GetOwner()->HasAuthority())
	Multiplier = NewMultiplier;
}

void ULimenCreditsComponent::CreditsUpdated()
{
	OnCreditsUpdated.Broadcast(this, GetCreditsString());
}

void ULimenCreditsComponent::OnRep_CurrentCredits()
{
	CreditsUpdated();
}
