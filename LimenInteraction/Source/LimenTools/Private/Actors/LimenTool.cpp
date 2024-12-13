// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenTool.h"

#include "TimerManager.h"
#include "Actors/LimenBattery.h"
#include "Attributes/LimenBatteryAttribute.h"
#include "Components/LimenAbilityComponent.h"
#include "Engine/World.h"


ALimenTool::ALimenTool()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = 0.03125;
	
	bStartActive = false;
	BatteryRechargeDelaySeconds = 1.f;
	CompatibleBatteryClass = nullptr;
	bIsRecharging = false;
	bIsActive = false;
	
	AbilityComponent = CreateDefaultSubobject<ULimenAbilityComponent>(TEXT("AbilityComponent"));
}

void ALimenTool::BeginPlay()
{
	Super::BeginPlay();

	AbilityComponent->LoadAbilities(this);
	AbilityComponent->LoadAttributes(this);
	
	BatteryAttribute = AbilityComponent->GetAttribute<ULimenBatteryAttribute>();
	check(BatteryAttribute != nullptr);
	BatteryAttribute->SetRechargeRate(DrainPerSecond > 0 ? -DrainPerSecond : DrainPerSecond);
	BatteryAttribute->OnAttributeEmpty.AddUniqueDynamic(this, &ThisClass::ALimenTool::BatteryEmpty);

	if (bStartActive)
	{
		ToolActivated();
	}
	else
	{
		ToolDeactivated();
	}
}

bool ALimenTool::ActivateTool()
{
	if (!CanActivate())
	{
		return false;
	}
	
	bIsActive = true;
	ToolActivated();
	BP_OnToolActivated();

	OnToolActiveStateChanged.Broadcast(true);
	return true;
}

void ALimenTool::DeactivateTool()
{
	bIsActive = false;
	ToolDeactivated();
	BP_OnToolDeactivated();

	OnToolActiveStateChanged.Broadcast(false);
}

bool ALimenTool::Recharge(ALimenBattery* NewBattery)
{
	check(NewBattery != nullptr);
	LastUsedBattery = NewBattery;
	
	DeactivateTool();
	OnBatteryRecharge(BatteryRechargeDelaySeconds);
	bIsRecharging = true;
	GetWorld()->GetTimerManager().SetTimer(RechargeTimer, this, &ThisClass::RechargeFinished, BatteryRechargeDelaySeconds);
	
	return true;
}

float ALimenTool::GetCurrentBatteryPercentage() const
{
	return BatteryAttribute->GetCurrentValueAsPercentage();
}

bool ALimenTool::CanActivate() const
{
	if (BatteryAttribute->IsEmpty())
	{
		return false;
	}
	if (bIsRecharging)
	{
		return false;
	}

	return true;
}

void ALimenTool::DataLoaded()
{
	Super::DataLoaded();
	
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	BatteryAttribute->SetCurrentValueAs(CurrentBatteryPercentage);
	BatteryAttribute->OnAttributeChanged.AddUniqueDynamic(this, &ThisClass::CurrentBatteryCapacityChanged);
	OnBatteryChanged.Broadcast(BatteryAttribute->GetCurrentValue());
}

void ALimenTool::ToolActivated()
{
	BatteryAttribute->FreezeAttribute(false);
}

void ALimenTool::ToolDeactivated()
{
	BatteryAttribute->FreezeAttribute(true);
}

void ALimenTool::RechargeFinished()
{
	BatteryAttribute->SetCurrentValueAsMax();
	bIsRecharging = false;
	OnBatteryChanged.Broadcast(BatteryAttribute->GetCurrentValue());
	ActivateTool();
}

void ALimenTool::CurrentBatteryCapacityChanged(const float NewValue)
{
	CurrentBatteryPercentage = NewValue;
}

void ALimenTool::BatteryEmpty(const float NewValue)
{
	DeactivateTool();
}
