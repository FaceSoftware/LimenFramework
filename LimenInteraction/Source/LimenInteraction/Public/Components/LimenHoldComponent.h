// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenHoldComponent.generated.h"


class ILimenHoldableItem;

USTRUCT()
struct FLimenItemHoldComponent_ReplicatedData
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AActor> Current;
	UPROPERTY()
	TObjectPtr<AActor> Previous;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FPhysicalItemChange, AActor*, Old, AActor*, New);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenHoldComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FPhysicalItemChange OnItemChanged;

	ULimenHoldComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void Hold(AActor* InActor);
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	void StopHolding();
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	bool IsHoldingSomething() const;

	template<typename T>
	FORCEINLINE bool IsHoldingSomething() const
	{
		if (!IsHoldingSomething()) return false;
		return GetActor<T>() != nullptr;
	}

	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	AActor* GetActor() const;

	template<typename T>
	FORCEINLINE T* GetActor() const
	{
		return Cast<T>(GetActor());
	}
	template<typename T>
	FORCEINLINE T* GetActorChecked() const
	{
		return CastChecked<T>(GetActor());
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
