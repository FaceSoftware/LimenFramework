// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenDamageComponent.h"


ULimenDamageComponent::ULimenDamageComponent()
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bRunOnAnyThread = false;
	bAutoActivate = true;
}

void ULimenDamageComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	ActiveDamageInfo.Empty();
	DamageInstances.Empty();
}

void ULimenDamageComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!GetOwner()->HasAuthority()) { return; }

	TArray<FDamageInfo> TickDamageInfo;
	TickDamageInfo.Reserve(ActiveDamageInfo.Num());

	for (int32 i = ActiveDamageInfo.Num() - 1; i >= 0; --i)
	{
		check(ActiveDamageInfo.IsValidIndex(i))
		const FDamageInfo& Info = ActiveDamageInfo[i];

		// Process the damage with the damage type
		float RawDamage = Info.DamageParameters.DamageValue; // Default to the damage parameter
		if (Info.DamageType.IsValid())
		{
			RawDamage = Info.DamageType->ProcessRawDamage(DeltaTime, Info.DamageParameters);
		}

		// Create new parameters with the processed damage type
		FDamageParameters NewDamageParameters = Info.DamageParameters;
		NewDamageParameters.DamageValue = RawDamage;
		
		const float PostProcessedDamage = DamageCalcFunc
			? DamageCalcFunc(NewDamageParameters, Info.DamageType.Get())
			: RawDamage;

		NewDamageParameters.DamageValue = PostProcessedDamage;

		TickDamageInfo.Push(Info);

		// Check if we should stop applying the damage
		if (!Info.DamageType.IsValid() || Info.DamageType->ShouldStopApplyingDamage())
		{
			check(ActiveDamageInfo.IsValidIndex(i))

			DamageInstances.Remove(TStrongObjectPtr(Info.DamageType.Get()));
			ActiveDamageInfo.RemoveAt(i);
		}
	}

	for (const FDamageInfo& Info : TickDamageInfo)
	{
		Multicast_BroadcastDamageReceived(Info);
	}
}

void ULimenDamageComponent::ApplyDamage(AController* Instigator, AActor* Causer,
										const TSubclassOf<ULimenDamageType>& DamageType,
										const FDamageParameters& DamageParams)
{
	check(GetOwner()->HasAuthority())

	ULimenDamageType* DamageTypeInstance = nullptr;
	if (DamageType.Get())
	{
		DamageTypeInstance = NewObject<ULimenDamageType>(this, DamageType);
	}

	FDamageInfo Info;
	Info.Instigator = Instigator;
	Info.Causer = Causer;
	Info.DamageTypeClass = DamageType;
	Info.DamageType = DamageTypeInstance;
	Info.DamageParameters = DamageParams;

	DamageInstances.Push(TStrongObjectPtr(DamageTypeInstance));
	ActiveDamageInfo.Push(Info);
}

void ULimenDamageComponent::SetDamageCalculationFunction(
	const TFunction<float(const FDamageParameters&, const ULimenDamageType*)>& InFunction)
{
	DamageCalcFunc = InFunction;
}

void ULimenDamageComponent::Multicast_BroadcastDamageReceived_Implementation(const FDamageInfo& Info)
{
	// Broadcast the damage received
	OnDamageReceived.Broadcast(Info.Instigator.Get(), Info.Causer.Get(),
							   Info.DamageTypeClass, Info);
}
