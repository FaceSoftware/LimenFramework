// Copyright Face Software. All Rights Reserved.


#include "Triggers/LimenNotificationTrigger.h"

#include "Components/LimenNotificationComponent.h"
#include "GameFramework/HUD.h"


ALimenNotificationTrigger::ALimenNotificationTrigger()
{
	Notification.DisplayTime = 0.f;
}

void ALimenNotificationTrigger::ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const auto* Pawn = Cast<APawn>(OtherActor);
	if (!Pawn)
	{
		return;
	}

	const auto* PlayerController = Cast<APlayerController>(Pawn->GetController());
	if (!IsValid(PlayerController))
	{
		return;
	}
	
	const auto NotificationComponent = PlayerController->GetHUD()->GetComponentByClass<ULimenNotificationComponent>();
	if (!IsValid(NotificationComponent))
	{
		return;
	}

	const TSharedRef<FLimenNotification> NotificationRef = MakeShared<FLimenNotification>(Notification);
	NotificationComponent->QueueNotification(NotificationRef);
	Super::ComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);
}
