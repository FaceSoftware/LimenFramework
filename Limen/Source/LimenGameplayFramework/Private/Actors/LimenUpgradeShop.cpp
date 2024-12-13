// Copyright Face Software. All Rights Reserved.


#include "Actors/LimenUpgradeShop.h"

#include "GameFramework/HUD.h"
#include "HUDs/LimenHUD.h"


ALimenUpgradeShop::ALimenUpgradeShop(const FObjectInitializer& InObjectInitializer) : Super(InObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
}

void ALimenUpgradeShop::DisplayUpgradeShop(APlayerController* PlayerController, APawn* InPawn) const
{
	if (!PlayerController->IsValidLowLevelFast())
	{
		return;
	}

	auto* HUD = PlayerController->GetHUD();
	if (!HUD->IsValidLowLevelFast())
	{
		return;
	}

	auto* LimenHUD = Cast<ALimenHUD>(HUD);
	if (!IsValid(LimenHUD))
	{
		return;
	}
	
	LimenHUD->ShowItemSmithWidget();
}
