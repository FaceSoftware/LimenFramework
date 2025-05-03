// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenPhysicalItemHoldComponent.generated.h"

class ALimenPhysicalItem;


USTRUCT()
struct FLimenPhysicalItemHoldComponent_WeaponData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ALimenPhysicalItem> PhysicalItem;
	UPROPERTY()
	TObjectPtr<ALimenPhysicalItem> PreviousPhysicalItem;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPhysicalItemChange, ALimenPhysicalItem*, Old, ALimenPhysicalItem*, New);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenPhysicalItemHoldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPhysicalItemChange OnItemChanged;

	ULimenPhysicalItemHoldComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void Hold(ALimenPhysicalItem* InPhysicalItem);
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void StopHolding();
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	bool IsHoldingSomething() const;
	template<typename T>
	bool IsHoldingSomething() const
	{
		return Cast<T>(GetPhysicalItem()) != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	ALimenPhysicalItem* GetPhysicalItem() const;

	template<typename T>
	T* GetPhysicalItem() const
	{
		static_assert(TIsDerivedFrom<T, ALimenPhysicalItem>::Value, "T must be a derived from ALimenPhysicalItem");
		return Cast<T>(GetPhysicalItem());
	}

private:
	UPROPERTY(ReplicatedUsing=OnRep_WeaponData)
	FLimenPhysicalItemHoldComponent_WeaponData WeaponData;

	UFUNCTION()
	void OnRep_WeaponData();
};
