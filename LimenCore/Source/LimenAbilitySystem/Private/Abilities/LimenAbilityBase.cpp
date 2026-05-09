// Copyright Face Software. All Rights Reserved.


#include "Abilities/LimenAbilityBase.h"

#include "Components/LimenAbilityComponent.h"
#include "Engine/NetDriver.h"
#include "GameFramework/Actor.h"


ULimenAbilityBase::ULimenAbilityBase() : Super(), FTickableGameObject(ETickableTickType::Never)
{
	Owner = nullptr;
	bIsInitialized = false;
	bShouldForcefullyDeactivate = false;
}

void ULimenAbilityBase::PostInitProperties()
{
	Super::PostInitProperties();
	
	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		check(IsInGameThread())
		SetTickableTickType(ETickableTickType::Conditional);
	}
}

bool ULimenAbilityBase::IsSupportedForNetworking() const
{
	// return Super::IsSupportedForNetworking();
	return true;
}

int32 ULimenAbilityBase::GetFunctionCallspace(UFunction* Function, FFrame* Stack)
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

bool ULimenAbilityBase::CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack)
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

void ULimenAbilityBase::Initialize(AActor* InOwner)
{
	check(InOwner != nullptr);
	Owner = InOwner;

	AbilityComponent = Owner->GetComponentByClass<ULimenAbilityComponent>();
	bIsInitialized = true;
}

bool ULimenAbilityBase::IsInitialized() const
{
	return bIsInitialized;
}

void ULimenAbilityBase::Deinitialize(AActor* InOwner)
{
	bIsInitialized = false;
}

void ULimenAbilityBase::ForceDeactivateAbility()
{
	bShouldForcefullyDeactivate = true;
}

bool ULimenAbilityBase::ShouldForcefullyDeactivate() const
{
	return bShouldForcefullyDeactivate;
}

void ULimenAbilityBase::Tick(float DeltaTime)
{
}

ETickableTickType ULimenAbilityBase::GetTickableTickType() const
{
	return HasAnyFlags(RF_ClassDefaultObject) ? ETickableTickType::Never : ETickableTickType::Conditional;
}

bool ULimenAbilityBase::IsTickable() const
{
	return bIsInitialized;
}

TStatId ULimenAbilityBase::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULimenAbilityBase, STATGROUP_Tickables);
}

bool ULimenAbilityBase::IsTickableWhenPaused() const
{
	return false;
}

bool ULimenAbilityBase::IsTickableInEditor() const
{
	return false;
}

UWorld* ULimenAbilityBase::GetTickableGameObjectWorld() const
{
	return GetWorld();
}

ULimenAbilityComponent* ULimenAbilityBase::GetOwnerAbilityComponent() const
{
	return AbilityComponent.Get();
}

AActor* ULimenAbilityBase::GetOwner() const
{
	return Owner.Get();
}
