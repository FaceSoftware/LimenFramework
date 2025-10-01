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

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FZoomDelegate, ULimenCameraComponent*, CameraComponent, float, Value);

/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FZoomDelegate OnCameraZoomValueChanged;
	UPROPERTY(BlueprintAssignable)
	FZoomDelegate OnFirstPersonZoomValueChanged;

	ULimenCameraComponent();
	virtual void BeginPlay() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	FORCEINLINE void SetTiltEnabled(const bool bEnabled) { bIsTiltEnabled = bEnabled; }
	FORCEINLINE void NotifyYawInput(const float InputValue) { CurrentTilt = TiltFunctionPtr(TiltStrength * (bInvertTilt ? -InputValue : InputValue)); }
	UFUNCTION(BlueprintCallable)
	virtual void AddCameraZoom(float Amount);
	virtual void SetCameraZoom(float Amount);
	UFUNCTION(BlueprintCallable)
	virtual void AddFirstPersonZoom(float Amount);
	virtual void SetFirstPersonZoom(float Amount);
	UFUNCTION(BlueprintCallable)
	FFloatRange GetCameraZoomRange() const;
	UFUNCTION(BlueprintCallable)
	float GetCurrentCameraZoom() const;
	UFUNCTION(BlueprintCallable)
	float GetCurrentFirstPersonZoom() const;
	
	float CameraZoomToFOV(float InZoom);
	float FirstPersonZoomToFOV(float InZoom);
    float CameraFOVToZoom(float InFOV);
    float FirstPersonFOVToZoom(float InFOV);
	
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zoom")
	FFloatRange CameraZoomRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zoom")
	FFloatRange FirstPersonZoomRange;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zoom")
	float ZoomMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zoom", meta=(ClampMin=0))
	float CameraZoomInterpolationSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Zoom", meta=(ClampMin=0))
	float FirstPersonZoomInterpolationSpeed;
	
private:
	TWeakObjectPtr<APlayerController> PlayerController;
	FRotator OriginalRelativeRotation;

	float CurrentTilt;
	bool bIsTiltEnabled;

	bool bOriginalUsePawnControlRotation;
	TFunction<float(float)> TiltFunctionPtr;

	float BaseCameraFOV;
	float TargetCameraFOV;
	float BaseFirstPersonFOV;
	float TargetFirstPersonFOV;

	void SetTiltFunctionPtr();

	float CalculateLinearTilt(const float Target = 0.f);
	float CalculateEaseInTilt(const float Target = 0.f);

	float ZoomToFOV(float InZoom, const FFloatRange& FOVRange);
	float FOVToZoom(float InFOV, const FFloatRange& FOVRange);
};
