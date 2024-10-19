// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LimenCameraTiltComponent.generated.h"

/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenCameraTiltComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	ULimenCameraTiltComponent();
	virtual void OnComponentCreated() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void NotifyYawInput(const float InputValue);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	float MaxCameraTilt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	float CameraTiltRecoverSpeed;

private:
	FRotator PreviousComponentRotation;
	UPROPERTY()
	TWeakObjectPtr<APawn> OwnerPawn;

	float CurrentTilt;

	void CalculateCurrentTilt(const float DeltaTime);
};
