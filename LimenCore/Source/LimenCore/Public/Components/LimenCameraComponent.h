// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LimenCameraComponent.generated.h"


UENUM(BlueprintType)
enum class ETiltFunction : uint8
{
	Linear,
	EaseIn,
};

/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	ULimenCameraComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void SetTiltEnabled(const bool bEnabled);
	void NotifyYawInput(const float InputValue);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	ETiltFunction TiltFunction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float MaxCameraTilt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float CameraTiltRecoverSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	bool bInvertTilt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	bool bEnableTilt;
	
private:
	TWeakObjectPtr<APlayerController> PlayerController;
	FRotator OriginalRelativeRotation;

	float CurrentTilt;
	bool bIsTiltEnabled;

	bool bOriginalUsePawnControlRotation;
	TFunction<void(float)> TiltFunctionPtr;

	void SetTiltFunctionPtr();

	void CalculateLinearTilt(const float Target = 0.f);
	void CalculateEaseInTilt(const float Target = 0.f);
};
