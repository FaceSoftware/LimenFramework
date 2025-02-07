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
	
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void Hold(ALimenPhysicalItem* InPhysicalItem);
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void StopHolding();
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void Drop();
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	bool IsHoldingSomething() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	ALimenPhysicalItem* GetPhysicalItem() const;

private:
	bool bIsHoldingSomething;

	UPROPERTY()
	TObjectPtr<ALimenPhysicalItem> PhysicalItem;
};
