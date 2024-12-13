// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ChildActorComponent.h"
#include "LimenPhysicalItemHoldComponent.generated.h"

class ALimenPhysicalItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPhysicalItemChange, ALimenPhysicalItem*, Old, ALimenPhysicalItem*, New);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenPhysicalItemHoldComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPhysicalItemChange OnItemChanged;
	
	virtual void BeginPlay() override;
	
	void Hold(ALimenPhysicalItem* InPhysicalItem);
	void StopHolding();
	void Drop();
	bool IsHoldingSomething() const;
	
	ALimenPhysicalItem* GetPhysicalItem() const { return PhysicalItem.Get(); }

private:
	bool bIsHoldingSomething;

	UPROPERTY()
	TObjectPtr<ALimenPhysicalItem> PhysicalItem;
};
