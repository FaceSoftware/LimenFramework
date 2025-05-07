// Copyright FaceSoftware. All Rights Reserved.


#include "Upgrades/LimenUpgrade.h"

#include "BlueprintLibraries/LimenCoreStatics.h"
#include "Components/LimenCreditsComponent.h"
#include "Curves/CurveFloat.h"
#include "Engine/NetDriver.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"


ULimenUpgrade::ULimenUpgrade()
{
	UpgradeName = FText::FromString(TEXT("DefaultUpgradeName"));
	UpgradeDescription = FText::FromString(TEXT("DefaultUpgradeDescription"));
	MaxLevel = 0;
	CurrentLevel = 0;
}

void ULimenUpgrade::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, CurrentLevel,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, BoundController,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, BoundPawn,
		FDoRepLifetimeParams(COND_None, REPNOTIFY_OnChanged, true))
}

bool ULimenUpgrade::IsSupportedForNetworking() const
{
	// Super::IsSupportedForNetworking();
	return true;
}

int32 ULimenUpgrade::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
{
	// return Super::GetFunctionCallspace(Function, Stack);

	if (Function->FunctionFlags & FUNC_Static || !(Function->FunctionFlags & FUNC_Net))
	{
		return FunctionCallspace::Local;
	}

	bool bIsOnServer = false;
	if (const AActor* Owner = Cast<AActor>(GetOuter()))
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

bool ULimenUpgrade::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
{
	// return Super::CallRemoteFunction(Function, Parameters, OutParms, Stack);

	if (!Function || !GetOuter())
	{
		return false;
	}

	AActor* Owner = Cast<AActor>(GetOuter());
	if (!Owner)
	{
		return false;
	}

	UNetDriver* NetDriver = Owner->GetNetDriver();
	if (!NetDriver)
	{
		return false;
	}

	NetDriver->ProcessRemoteFunction(Owner, Function, Parameters, OutParms, Stack, this);
	return true;
}

void ULimenUpgrade::ProcessUpgrade(AController* InController, APawn* InPawn, ULimenCreditsComponent* CreditsComponent)
{
	check(CanApplyUpgrade(InController, InPawn, CreditsComponent))

	BoundController = InController;
	BoundPawn = InPawn;

	const int32 NextLevel = CurrentLevel + 1;
	const float NewCostValue = GetCost(NextLevel);
	verify(CreditsComponent->WithdrawCredits(NewCostValue))

	const float NewUpgradeValue = GetUpgradeValue(NextLevel);
	Multicast_ProcessUpgrade(InController, InPawn, NewUpgradeValue);

	CurrentLevel = NextLevel;
	CurrentLevelChanged();
}

bool ULimenUpgrade::CanApplyUpgrade(AController* InController, APawn* InPawn, const ULimenCreditsComponent* CreditsComponent) const
{
	if (MaxLevel != 0 && CurrentLevel >= MaxLevel)
	{
		return false;
	}

	if (!CostFunction || !UpgradeFunction)
	{
		return false;
	}
	
	return InController != nullptr && InPawn != nullptr && CreditsComponent != nullptr &&
		   CreditsComponent->GetCredits() >= GetCost(CurrentLevel + 1);
}

int32 ULimenUpgrade::GetCurrentUpgradeLevel() const
{
	return CurrentLevel;
}

int32 ULimenUpgrade::GetCost(const int32 Level) const
{
	return CostFunction->GetFloatValue(Level);
}

float ULimenUpgrade::GetUpgradeValue(const int32 Level) const
{
	return UpgradeFunction->GetFloatValue(Level);
}

void ULimenUpgrade::ProcessUpgradeInternal(AController* InController, APawn* InPawn, float NewUpgradeValue)
{
}

void ULimenUpgrade::CurrentLevelChanged()
{
	OnUpgradeApplied.Broadcast(this);
}

void ULimenUpgrade::OnRep_CurrentLevel()
{
	CurrentLevelChanged();
}

void ULimenUpgrade::Multicast_ProcessUpgrade_Implementation(AController* InController, APawn* InPawn, const float NewUpgradeValue)
{
	ProcessUpgradeInternal(InController, InPawn, NewUpgradeValue);
}
