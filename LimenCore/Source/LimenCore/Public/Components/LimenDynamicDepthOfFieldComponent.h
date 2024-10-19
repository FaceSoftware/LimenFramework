// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenDynamicDepthOfFieldComponent.generated.h"


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
	
protected:
	void FindGlobalPostProcess();
	UFUNCTION()
	virtual void GlobalPostProcessFound();
	
	float GetLookAtDistance() const;

private:
	UPROPERTY(EditDefaultsOnly)
	FName GlobalPostProcessTag;
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> LineTraceChannel;
	UPROPERTY()
	TObjectPtr<APawn> OwnerPawn;
	UPROPERTY()
	TObjectPtr<APlayerController> OwnerPlayerController;

	FCollisionQueryParams QueryParams;

	UPROPERTY()
	TObjectPtr<APostProcessVolume> GlobalPostProcess;

	UPROPERTY()
	TObjectPtr<ULimenRecurrentAction> FindGlobalPostProcessAction;

	UFUNCTION()
	void SetPostProcess();

	UFUNCTION()
	bool HasFoundGlobalPostProcess();
};
