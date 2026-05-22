// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenTrigger.h"
#include "CppClasses/LimenNotification.h"
#include "LimenNotificationTrigger.generated.h"


UCLASS(Blueprintable)
class LIMENTRIGGERS_API ALimenNotificationTrigger : public ALimenTrigger
{
	GENERATED_BODY()

public:
	ALimenNotificationTrigger();

protected:
	virtual void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;;

private:
	UPROPERTY(EditAnywhere, Category="Limen|Notifications")
	FNotificationParams Notification;

};
