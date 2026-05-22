// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "Components/ActorComponent.h"
#include "LimenDynamicDepthOfFieldComponent.generated.h"


struct FPostProcessSettings;
struct FCollisionQueryParams;
class ULimenRecurrentAction;
class APostProcessVolume;
class UCameraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenDynamicDepthOfFieldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenDynamicDepthOfFieldComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;
	
protected:	
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> LineTraceChannel;
	UPROPERTY(EditDefaultsOnly)
	bool bTraceIgnoresOwner;
	
	FCollisionQueryParams QueryParams;
	
	float GetLookAtDistance() const;

private:
	static constexpr float MaxFocalDistance = 100000.f;
	
	TArray<UCameraComponent*> OwnerCameras;
	TWeakObjectPtr<UCameraComponent> ActiveCamera;
	FPostProcessSettings* CurrentCameraPostProcessSettings;

	UFUNCTION()
	void ActiveCameraChanged(UActorComponent* Component, bool bReset);

	bool IsAnyCameraViewTarget() const;
};
