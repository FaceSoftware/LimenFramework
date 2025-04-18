// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Components/LimenDamageComponent.h"


ULimenDamageComponent::ULimenDamageComponent()
{
	
}

void ULimenDamageComponent::BeginPlay()
{
	Super::BeginPlay();
}

void ULimenDamageComponent::TickComponent(const float DeltaTime, const ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (int32 i = ActiveDamageInfo.Num() - 1; i >= 0; ++i)
	{
		const FDamageInfo& Info = ActiveDamageInfo[i];

		// Process the damage with the damage type
		const float RawDamage = Info.DamageType->ProcessRawDamage(DeltaTime, Info.DamageParameters);

		// Check if we should stop applying the damage
		if (Info.DamageType->ShouldStopApplyingDamage())
		{
			ActiveDamageInfo.RemoveAt(i);
			continue;
		}

		// Create new parameters with the processed damage type
		FDamageParameters NewDamageParameters(RawDamage);
		NewDamageParameters.DamageValue = RawDamage;
		const float PostProcessedDamage = DamageCalcFunc(NewDamageParameters, Info.DamageType.Get());

		// Broadcast the damage received
		OnDamageReceived.Broadcast(Info.Instigator.Get(), Info.Causer.Get(),
								   Info.DamageType.Get(), PostProcessedDamage);
	}
}

void ULimenDamageComponent::ApplyDamage(AController* Instigator, AActor* Causer,
										const TSubclassOf<ULimenDamageType>& DamageType,
										const FDamageParameters& DamageParams)
{
	ULimenDamageType* DamageTypeInstance = nullptr;
	if (DamageType.Get())
	{
		DamageTypeInstance = NewObject<ULimenDamageType>(this, DamageType);
	}

	FDamageInfo Info;
	Info.Instigator = Instigator;
	Info.Causer = Causer;
	Info.DamageType = TStrongObjectPtr(DamageTypeInstance);
	Info.DamageParameters = DamageParams;

	ActiveDamageInfo.Push(Info);
}
