// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenBattery.h"

#include "Attributes/LimenBatteryAttribute.h"


ALimenBattery::ALimenBattery()
{
	PrimaryActorTick.bCanEverTick = false;
	Capacity = CreateDefaultSubobject<ULimenBatteryAttribute>("BatteryAttribute");
	check(Capacity);
}

void ALimenBattery::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Capacity->Initialize();
}

void ALimenBattery::BeginPlay()
{
	Super::BeginPlay();
}

bool ALimenBattery::ShouldLoadData() const
{
	if (Super::ShouldLoadData())
	{
		return false;
	}
	
	return true;
}

bool ALimenBattery::ShouldSaveData() const
{
	if (Super::ShouldSaveData())
	{
		return false;
	}
	
	return true;
}

bool ALimenBattery::IsEmpty() const
{
	return Capacity->IsEmpty();
}

float ALimenBattery::GetPercentage() const
{
	if (!Capacity)
	{
		return 0;
	}
	
	return Capacity->GetCurrentValueAsPercentage();
}

void ALimenBattery::Drain(const float Value)
{
	Capacity->ModifyValueBy(-FMath::Abs(Value));
}

ULimenBatteryAttribute* ALimenBattery::GetBatteryCapacity() const
{
	return Capacity.Get();
}
