// Copyright Face Software. All Rights Reserved.


#include "Abilities/LimenAbilityBase.h"

#include "Components/LimenAbilityComponent.h"
#include "GameFramework/Actor.h"


ULimenAbilityBase::ULimenAbilityBase() : Super()
{
	Owner = nullptr;
	bIsInitialized = false;
	bShouldForcefullyDeactivate = false;
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

void ULimenAbilityBase::ForceDeactivateAbility()
{
	bShouldForcefullyDeactivate = true;
}

void ULimenAbilityBase::Tick(float DeltaTime)
{
}

ETickableTickType ULimenAbilityBase::GetTickableTickType() const
{
	return ETickableTickType::Conditional;
}

bool ULimenAbilityBase::IsTickable() const
{
	return !HasAnyFlags(RF_ClassDefaultObject);
}

bool ULimenAbilityBase::IsAllowedToTick() const
{
	return !bShouldForcefullyDeactivate;
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
