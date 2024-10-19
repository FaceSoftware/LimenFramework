// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenDiscardableItem.h"
#include "LimenBattery.generated.h"

class ULimenBatteryAttribute;
class ULimenAttributeComponent;

UCLASS()
class LIMENTOOLS_API ALimenBattery : public ALimenDiscardableItem
{
	GENERATED_BODY()

public:	
	ALimenBattery();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;

	virtual bool ShouldLoadData() const override;
	virtual bool ShouldSaveData() const override;
	
	bool IsEmpty() const;
	float GetPercentage() const;
	void Drain(const float Value);

	ULimenBatteryAttribute* GetBatteryCapacity() const;
	
private:
	UPROPERTY()
	TObjectPtr<ULimenBatteryAttribute> Capacity;
};
