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
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	void SetTiltEnabled(const bool bEnabled);
	void NotifyYawInput(const float InputValue);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tilt", meta=(EditCondition="bEnableTilt"))
	ETiltFunction TiltFunction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tilt", meta=(ClampMin="0", EditCondition="bEnableTilt"))
	float MaxCameraTilt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tilt", meta=(ClampMin="0", EditCondition="bEnableTilt"))
	float TiltStrength;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tilt", meta=(ClampMin="0", EditCondition="bEnableTilt"))
	float CameraTiltRecoverSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tilt", meta=(EditCondition="bEnableTilt"))
	bool bInvertTilt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tilt")
	bool bEnableTilt;
	
private:
	TWeakObjectPtr<APlayerController> PlayerController;
	FRotator OriginalRelativeRotation;

	float CurrentTilt;
	bool bIsTiltEnabled;

	bool bOriginalUsePawnControlRotation;
	TFunction<float(float)> TiltFunctionPtr;

	void SetTiltFunctionPtr();

	float CalculateLinearTilt(const float Target = 0.f);
	float CalculateEaseInTilt(const float Target = 0.f);
};
