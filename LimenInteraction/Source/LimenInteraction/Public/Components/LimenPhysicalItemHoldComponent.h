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
	
	virtual void Hold(ALimenPhysicalItem* InPhysicalItem);
	virtual void StopHolding();
	bool IsHoldingSomething() const;

	UFUNCTION(BlueprintCallable)
	ALimenPhysicalItem* GetPhysicalItem() const;

	template<typename T>
	T* GetPhysicalItem() const
	{
		static_assert(std::is_base_of_v<ALimenPhysicalItem, T>);
		return Cast<T>(GetPhysicalItem());
	}

	TSubclassOf<ALimenPhysicalItem> GetPhysicalItemParentClass() const;

private:
	bool bIsHoldingSomething;

	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TSoftClassPtr<ALimenPhysicalItem> PhysicalItemParentClass;

	UPROPERTY()
	TObjectPtr<ALimenPhysicalItem> PhysicalItem;
};
