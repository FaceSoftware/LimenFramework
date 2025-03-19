// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenMouseSensitivityComponent.generated.h"


USTRUCT(BlueprintType)
struct FMouseParameters
{
	GENERATED_BODY()

	FMouseParameters() : MouseX(false, 1.f), MouseY(false, 1.f) {}

	explicit FMouseParameters(const float X, const float Y, const bool bInvertX = false, const bool bInvertY = false)
		: MouseX(bInvertX, X), MouseY(bInvertY, Y) {}

	explicit FMouseParameters(const float Sensitivity, const bool bInvert = false)
		: MouseX(bInvert, Sensitivity), MouseY(bInvert, Sensitivity) {}

	TPair<bool, float> MouseX;
	TPair<bool, float> MouseY;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseSensitivityUpdate, ULimenMouseSensitivityComponent*, SensitivityComponent);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenMouseSensitivityComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMouseSensitivityUpdate OnSensitivityUpdated;
	
	void SetMouseParameters(const FMouseParameters& InNewParams);
	void SetSensitivity(const float Sensitivity);
	void SetInvertAxis(const bool bInvertX, const bool bInvertY);

	float GetMouseX() const;
	bool GetInvertMouseX() const;

	float GetMouseY() const;
	bool GetInvertMouseY() const;

	float GetMouseXY() const;

	const FMouseParameters& GetMouseParameters() const;
	
protected:

private:
	FMouseParameters MouseParameters;
};
