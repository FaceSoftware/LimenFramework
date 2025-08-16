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
	bWasActiveBeforeRecharge = false;
	
	AbilityComponent = CreateDefaultSubobject<ULimenAbilityComponent>(TEXT("AbilityComponent"));
	AbilityComponent->bAutoActivate = true;
}

void ALimenTool::BeginPlay()
{
	Super::BeginPlay();
	
	BatteryAttribute = AbilityComponent->GetAttribute<ULimenBatteryAttribute>();
	check(BatteryAttribute != nullptr);
	BatteryAttribute->SetRechargeRate(DrainPerSecond > 0 ? -DrainPerSecond : DrainPerSecond);
	BatteryAttribute->OnAttributeEmpty.AddUniqueDynamic(this, &ThisClass::BatteryEmpty);
	BatteryAttribute->OnAttributeChanged.AddUniqueDynamic(this, &ThisClass::CurrentBatteryCapacityChanged);

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

	bWasActiveBeforeRecharge = IsActive();
	
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

	const bool bIsFrozen = BatteryAttribute->IsFrozen();
	BatteryAttribute->FreezeAttribute(false);
	BatteryAttribute->SetValue(CurrentBatteryPercentage);
	BatteryAttribute->FreezeAttribute(bIsFrozen);
	
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
	check(BatteryAttribute->IsFrozen())
	
	BatteryAttribute->FreezeAttribute(false);
	BatteryAttribute->SetCurrentValueAsMax();
	BatteryAttribute->FreezeAttribute(true);
	
	bIsRecharging = false;
	OnBatteryChanged.Broadcast(BatteryAttribute->GetCurrentValue());

	if (bWasActiveBeforeRecharge)
	{
		ActivateTool();
	}
}

void ALimenTool::CurrentBatteryCapacityChanged(ULimenAttributeBase* Attribute, const float NewValue)
{
	CurrentBatteryPercentage = NewValue;
}

void ALimenTool::BatteryEmpty(ULimenAttributeBase* Attribute, const float NewValue)
{
	DeactivateTool();
}
