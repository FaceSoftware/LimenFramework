// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenItemHoldComponent.generated.h"


class ILimenHoldableItem;

USTRUCT()
struct FLimenItemHoldComponent_ReplicatedData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> PhysicalItem;
	UPROPERTY()
	TObjectPtr<AActor> PreviousPhysicalItem;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPhysicalItemChange, TScriptInterface<ILimenHoldableItem>, Old, TScriptInterface<ILimenHoldableItem>, New);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenItemHoldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPhysicalItemChange OnItemChanged;

	ULimenItemHoldComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void Hold(const TScriptInterface<ILimenHoldableItem>& InPhysicalItem);
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void StopHolding();
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	bool IsHoldingSomething() const;

	template<typename T>
	FORCEINLINE bool IsHoldingSomething() const
	{
		if (!IsHoldingSomething()) return false;
		return GetItem<T>() != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	TScriptInterface<ILimenHoldableItem> GetItem() const;

	template<typename T>
	FORCEINLINE T* GetItem() const
	{
		return Cast<T>(GetItem());
	}
	template<typename T>
	FORCEINLINE T* GetItemChecked() const
	{
		return CastChecked<T>(GetItem());
	}
	template<typename T>
	FORCEINLINE T* GetItemInterface() const
	{
		static_assert(TIsIInterface<T>::Value);
		return TScriptInterface<T>(GetItem().GetObject());
	}

private:
	UPROPERTY(ReplicatedUsing=OnRep_WeaponData)
	FLimenItemHoldComponent_ReplicatedData WeaponData;

	UFUNCTION()
	void OnRep_WeaponData(const FLimenItemHoldComponent_ReplicatedData& OldWeaponData);
	
	UFUNCTION(Server, Reliable)
	void Server_Hold(AActor* InItem);
	UFUNCTION(Server, Reliable)
	void Server_StopHolding();
	
	void Agnostic_Hold(AActor* InItem);
	void Agnostic_StopHolding();
};
