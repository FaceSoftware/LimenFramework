// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenPhysicalItemHoldComponent.generated.h"


class ALimenPhysicalItem;

USTRUCT()
struct FLimenPhysicalItemHoldComponent_ReplicatedWeaponData
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
	FORCEINLINE bool IsHoldingSomething() const
	{
		if (!IsHoldingSomething()) return false;

		return GetPhysicalItem<T>() != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	ALimenPhysicalItem* GetPhysicalItem() const;

	template<typename T>
	FORCEINLINE T* GetPhysicalItem() const
	{
		static_assert(TIsDerivedFrom<T, ALimenPhysicalItem>::Value, "T must be a derived from ALimenPhysicalItem");

		T* Item;
		if constexpr (std::is_same_v<T, ALimenPhysicalItem>)
		{
			Item = GetPhysicalItem();
		}
		else
		{
			Item = Cast<T>(GetPhysicalItem());
		}
		return Item;
	}
	template<typename T = ALimenPhysicalItem>
	FORCEINLINE T* GetPhysicalItemChecked() const
	{
		static_assert(TIsDerivedFrom<T, ALimenPhysicalItem>::Value, "T must be a derived from ALimenPhysicalItem");

		T* Item;
		if constexpr (std::is_same_v<T, ALimenPhysicalItem>)
		{
			Item = GetPhysicalItem();
		}
		else
		{
			Item = CastChecked<T>(GetPhysicalItem());
		}
		return Item;
	}

private:
	UPROPERTY(ReplicatedUsing=OnRep_WeaponData)
	FLimenPhysicalItemHoldComponent_ReplicatedWeaponData WeaponData;

	UFUNCTION()
	void OnRep_WeaponData();
};
