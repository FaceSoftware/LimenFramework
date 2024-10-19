// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenTool.h"
#include "LimenFlashlight.generated.h"


class ULimenConeTraceComponent;
class USpotLightComponent;
class ALimenBattery;

UCLASS()
class LIMENTOOLS_API ALimenFlashlight : public ALimenTool
{
	GENERATED_BODY()

public:	
	ALimenFlashlight();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	UFUNCTION(BlueprintCallable, Category="Limen|Flashlight")
	ULimenConeTraceComponent* GetConeTraceComponent() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetLight")
	TObjectPtr<USpotLightComponent> Light;

	UPROPERTY(EditDefaultsOnly, BlueprintGetter="GetConeTraceComponent")
	TObjectPtr<ULimenConeTraceComponent> ConeTrace;
	
	UFUNCTION(BlueprintCallable)
	USpotLightComponent* GetLight() const;

	virtual void ToolActivated() override;
	virtual void ToolDeactivated() override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnActorLitUpdate(AActor* Actor, const bool bIsInside);
	
private:
};
