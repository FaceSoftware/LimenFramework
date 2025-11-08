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
	PrimaryActorTick.TickInterval = 0.f;
	
	bStartActive = false;
	bIsActive = false;
	
	AbilityComponent = CreateDefaultSubobject<ULimenAbilityComponent>(TEXT("AbilityComponent"));
	AbilityComponent->bAutoActivate = true;
}

void ALimenTool::BeginPlay()
{
	Super::BeginPlay();

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

	OnToolActiveStateChanged.Broadcast(true);
	return true;
}

void ALimenTool::DeactivateTool()
{
	if (!IsActive()) { return; }

	bIsActive = false;
	ToolDeactivated();

	OnToolActiveStateChanged.Broadcast(false);
}

bool ALimenTool::ToggleTool()
{
	if (IsActive()) { DeactivateTool(); } else { ActivateTool(); }
	return IsActive();
}

bool ALimenTool::CanActivate() const
{
	return !IsActive();
}

bool ALimenTool::IsActive() const
{
	return bIsActive;
}

void ALimenTool::ToolActivated()
{
}

void ALimenTool::ToolDeactivated()
{
}
