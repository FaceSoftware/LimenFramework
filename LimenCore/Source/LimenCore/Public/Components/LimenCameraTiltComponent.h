// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "LimenCameraTiltComponent.generated.h"


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
class LIMENCORE_API ULimenCameraTiltComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	ULimenCameraTiltComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

#if WITH_EDITORONLY_DATA
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	void SetTiltEnabled(const bool bEnabled);
	void NotifyYawInput(const float InputValue);
	
protected:
	/**
	 * @brief Low values will have a less impact on the tilt.
	 * The default, 60, works best in all scenarios unless the game fps is lower than that.
	 * @warning This will override the Tick Interval
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	float TargetFps;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	ETiltFunction TiltFunction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float MaxCameraTilt;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen", meta=(ClampMin="0"))
	float CameraTiltRecoverSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	bool bTiltTowardsMovement;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	bool bEnableTilt;

	void CalculateCurrentTilt(const float DeltaTime);
	
private:
	TWeakObjectPtr<APlayerController> PlayerController;
	FRotator OriginalRelativeRotation;

	float CurrentTilt;
	bool bIsTiltEnabled;

	bool bOriginalUsePawnControlRotation;

	void CalculateLinearTilt(const float DeltaTime);
	void CalculateEaseInTilt(const float DeltaTime);
};
