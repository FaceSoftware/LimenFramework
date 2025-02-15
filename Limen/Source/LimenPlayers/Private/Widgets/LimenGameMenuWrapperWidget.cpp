// Copyright Face Software. All Rights Reserved.


#include "Widgets/LimenGameMenuWrapperWidget.h"

#include "Kismet/GameplayStatics.h"
#include "HUDs/LimenHUD.h"
#include "Widgets/LimenObjectiveInventory.h"


void ULimenGameMenuWrapperWidget::ReturnToGame()
{
	const auto* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	check(PlayerController)
	auto* LimenHUD = Cast<ALimenHUD>(PlayerController->GetHUD());
	check(LimenHUD)
	
	LimenHUD->HideGameMenuWidget();
}
