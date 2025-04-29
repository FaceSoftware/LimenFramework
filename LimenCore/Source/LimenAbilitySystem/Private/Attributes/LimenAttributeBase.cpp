// Copyright Face Software. All Rights Reserved.


#include "Attributes/LimenAttributeBase.h"

#include "Components/LimenAbilityComponent.h"
#include "Engine/NetDriver.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"
#include "Iris/ReplicationSystem/ReplicationSystem.h"
#include "Net/UnrealNetwork.h"
#include "Net/Iris/ReplicationSystem/ReplicationSystemUtil.h"


ULimenAttributeBase::ULimenAttributeBase() : Super()
{
	MaxValue = 100.f;
	InitialValue = 100.f;
	RechargeRate = 0.f;
	CurrentValue = 0.f;
	bIsInitialized = false;
	bIsFrozen = false;
}

void ULimenAttributeBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentValue,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

bool ULimenAttributeBase::IsSupportedForNetworking() const
{
	// Super::IsSupportedForNetworking();
	return true;
}

int32 ULimenAttributeBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	// return Super::GetFunctionCallspace(Function, Stack);

	if (Function->FunctionFlags & FUNC_Static || !(Function->FunctionFlags & FUNC_Net))
	{
		return FunctionCallspace::Local;
	}

	bool bIsOnServer = false;
	if (Owner.IsValid())
	{
		if (const UNetDriver* NetDriver = Owner->GetNetDriver())
		{
			bIsOnServer = NetDriver->IsServer();
		}
	}

	// get the top most function
	while (Function->GetSuperFunction() != nullptr)
	{
		Function = Function->GetSuperFunction();
	}

	// Multicast RPCs
	if ((Function->FunctionFlags & FUNC_NetMulticast))
	{
		if (bIsOnServer)
		{
			// Server should execute locally and call remotely
			return (FunctionCallspace::Local | FunctionCallspace::Remote);
		}
		else
		{
			return FunctionCallspace::Local;
		}
	}

	// if we are the authority
	if (bIsOnServer)
	{
		if (Function->FunctionFlags & FUNC_NetClient)
		{
			return FunctionCallspace::Remote;
		}
		else
		{
			return FunctionCallspace::Local;
		}

	}
	// if we are not the authority
	else
	{
		if (Function->FunctionFlags & FUNC_NetServer)
		{
			return FunctionCallspace::Remote;
		}
		else
		{
			// don't replicate
			return FunctionCallspace::Local;
		}
	}
}

bool ULimenAttributeBase::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms,
	FFrame* Stack)
{
	// return Super::CallRemoteFunction(Function, Parameters, OutParms, Stack);

	if (!Function || !GetOwner())
	{
		return false;
	}

	if (!Owner.IsValid())
	{
		return false;
	}

	UNetDriver* NetDriver = Owner->GetNetDriver();
	if (!NetDriver)
	{
		return false;
	}

	NetDriver->ProcessRemoteFunction(Owner.Get(), Function, Parameters, OutParms, Stack, this);
	return true;
}

bool ULimenAttributeBase::HasAuthority() const
{
	if (!Owner.IsValid())
	{
		return false;
	}

	return Owner->HasAuthority();
}

void ULimenAttributeBase::Initialize(AActor* InOwner)
{
	check(InOwner != nullptr);
	Owner = InOwner;
	OwnerAbilityComponent = Owner->GetComponentByClass<ULimenAbilityComponent>();
	SetCurrentValueAs(InitialValue);
	bIsInitialized = true;
}

void ULimenAttributeBase::Deinitialize(AActor* InOwner)
{
}

void ULimenAttributeBase::Initialize()
{
	CurrentValue = InitialValue;
	bIsInitialized = true;
}

bool ULimenAttributeBase::IsInitialized() const
{
	return bIsInitialized;
}

void ULimenAttributeBase::Tick(float DeltaTime)
{
	if (HasAuthority())
	{
		if (!FMath::IsNearlyZero(RechargeRate))
		{
			ModifyValueBy(RechargeRate * DeltaTime);
		}
	}
}

ETickableTickType ULimenAttributeBase::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULimenAttributeBase::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

bool ULimenAttributeBase::IsAllowedToTick() const
{
	return true;
}

TStatId ULimenAttributeBase::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULimenAttributeBase, STATGROUP_Tickables);
}

bool ULimenAttributeBase::IsTickableWhenPaused() const
{
	return false;
}

bool ULimenAttributeBase::IsTickableInEditor() const
{
	return false;
}

UWorld* ULimenAttributeBase::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

void ULimenAttributeBase::SetRechargeRate(const float Value)
{
	RechargeRate = Value;
}

void ULimenAttributeBase::SetValue(const float Value)
{
	if (Value > MaxValue)
	{
		SetCurrentValueAsMax();
	}
	else if (Value < MinValue)
	{
		SetCurrentValueAsMin();
	}
	else
	{
		SetCurrentValueAs(Value);
	}
}

void ULimenAttributeBase::ModifyValueBy(const float Value)
{
	if (const float Test = CurrentValue + Value;
		Test >= MaxValue)
	{
		SetCurrentValueAsMax();
	}
	else if (Test <= MinValue)
	{
		SetCurrentValueAsMin();
	}
	else
	{
		SetCurrentValueAs(Test);
	}
}

float ULimenAttributeBase::SafeModifyValueBy(const float Value)
{
	float Out = Value;
	if (const float Test = CurrentValue + Value;
		Test > MaxValue)
	{
		Out = MaxValue - CurrentValue;
		SetCurrentValueAsMax();
	}
	else if (Test < MinValue)
	{
		Out = CurrentValue;
		SetCurrentValueAsMin();
	}
	else
	{
		SetCurrentValueAs(Test);
	}

	FMath::Abs(Out);
	return Out;
}

void ULimenAttributeBase::ModifyValuePercentageBy(const float Percent)
{
	if (const float Test = CurrentValue + CurrentValue * Percent;
		Test > MaxValue)
	{
		SetCurrentValueAsMax();
	}
	else if (Test < MinValue)
	{
		SetCurrentValueAsMin();
	}
	else
	{
		SetCurrentValueAs(Test);
	}
}

void ULimenAttributeBase::ModifyMaxValueBy(const float Value)
{
	if (const float Test = MaxValue + Value;
		Test > MaxValue)	
	{
		SetCurrentValueAsMax();
	}
	else if (Test < MinValue)
	{
		SetCurrentValueAsMin();
	}
	else
	{
		SetCurrentValueAs(Test);
	}
}

void ULimenAttributeBase::ModifyMaxValuePercentageBy(const float Percent)
{
	if (const float Test = MaxValue + MaxValue * Percent;
		Test < MinValue)	
	{
		SetCurrentValueAsMax();
	}
	else if (Test < MinValue)
	{
		SetCurrentValueAsMin();
	}
	else
	{
		SetCurrentValueAs(Test);
	}
}

void ULimenAttributeBase::Empty()
{
	SetCurrentValueAsMin();
}

void ULimenAttributeBase::Full()
{
	SetCurrentValueAsMax();
}

float ULimenAttributeBase::GetMaxValue() const
{
	return MaxValue;
}

float ULimenAttributeBase::GetMinValue() const
{
	return MinValue;
}

float ULimenAttributeBase::GetCurrentValue() const
{
	return CurrentValue;
}

float ULimenAttributeBase::GetCurrentValueAsPercentage() const
{
	return CurrentValue * 100 / MaxValue;
}

bool ULimenAttributeBase::IsEmpty() const
{
	return FMath::IsNearlyEqual(CurrentValue, MinValue);
}

bool ULimenAttributeBase::IsFull() const
{
	return FMath::IsNearlyEqual(CurrentValue, MaxValue);
}

bool ULimenAttributeBase::StatsMakeSense() const
{
	return MaxValue > MinValue && CurrentValue >= MinValue && CurrentValue <= MaxValue;
}

void ULimenAttributeBase::SetCurrentValueAsMax()
{
	if (bIsFrozen || IsFull())
	{
		return;
	}
	
	SetCurrentValueAs(MaxValue);
	check(CurrentValue == MaxValue);
	AttributeFull();
	OnAttributeFull.Broadcast(this, CurrentValue);
}

void ULimenAttributeBase::SetCurrentValueAsMin()
{
	if (bIsFrozen || IsEmpty())
	{
		return;
	}
	
	SetCurrentValueAs(MinValue);
	check(CurrentValue == MinValue);
	AttributeEmpty();
	OnAttributeEmpty.Broadcast(this, CurrentValue);
}

void ULimenAttributeBase::SetCurrentValueAs(const float Value)
{
	if (IsFrozen() || FMath::IsNearlyEqual(CurrentValue, Value))
	{
		return;
	}
	
	CurrentValue = Value;
	AttributeUpdated();
	OnAttributeChanged.Broadcast(this, CurrentValue);
}

ULimenAbilityComponent* ULimenAttributeBase::GetOwnerAbilityComponent() const
{
	return OwnerAbilityComponent.Get();
}

AActor* ULimenAttributeBase::GetOwner() const
{
	return Owner.Get();
}

void ULimenAttributeBase::FreezeAttribute(const bool bShouldFreeze)
{
	bIsFrozen = bShouldFreeze;
}

bool ULimenAttributeBase::IsFrozen() const
{
	return bIsFrozen;
}

void ULimenAttributeBase::AttributeEmpty()
{
}

void ULimenAttributeBase::AttributeFull()
{
}

void ULimenAttributeBase::AttributeUpdated()
{
}

void ULimenAttributeBase::OnRep_CurrentValue()
{
	if (CurrentValue <= MinValue)
	{
		AttributeEmpty();
		OnAttributeEmpty.Broadcast(this, CurrentValue);
	}
	else if (CurrentValue >= MaxValue)
	{
		AttributeFull();
		OnAttributeFull.Broadcast(this, CurrentValue);
	}

	AttributeUpdated();
	OnAttributeChanged.Broadcast(this, CurrentValue);
}
