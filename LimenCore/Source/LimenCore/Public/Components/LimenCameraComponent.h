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

	FORCEINLINE void SetTiltEnabled(const bool bEnabled);
	FORCEINLINE void NotifyYawInput(const float InputValue);
	UFUNCTION(BlueprintCallable)
	virtual void CameraZoom(float Amount);
	UFUNCTION(BlueprintCallable)
	virtual void FirstPersonZoom(float Amount);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE FFloatRange GetCameraZoomRange() const;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentCameraZoom() const;
	UFUNCTION(BlueprintCallable)
	FORCEINLINE float GetCurrentFirstPersonZoom() const;
	/**
	 * @brief Current zoom in a [0, 1] range.
	 * @return Zero if zoom it at minimum. One if it is at maximum. Any other value returns the corresponding value in between.
	 */
	UFUNCTION(BlueprintCallable)
	float GetCurrentCameraZoomNormalized() const;
	/**
	 * @brief Current zoom in a [0, 1] range.
	 * @return Zero if zoom it at minimum. One if it is at maximum. Any other value returns the corresponding value in between.
	 */
	UFUNCTION(BlueprintCallable)
	float GetCurrentFirstPersonZoomNormalized() const;
	
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
