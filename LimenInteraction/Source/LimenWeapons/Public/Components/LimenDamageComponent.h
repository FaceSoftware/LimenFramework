// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageTypes/LimenDamageType.h"
#include "LimenDamageComponent.generated.h"

class ULimenDamageType;
class AController;


USTRUCT(BlueprintType)
struct FDamageInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AController> Instigator;
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AActor> Causer;
	UPROPERTY(BlueprintReadOnly)
	TSubclassOf<ULimenDamageType> DamageTypeClass;
	UPROPERTY(BlueprintReadOnly)
	FDamageParameters DamageParameters;

	TWeakObjectPtr<ULimenDamageType> DamageType;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLimenDamageEvent, AController*, Instigator,
	AActor*, Causer, TSubclassOf<ULimenDamageType>, DamageType, const FDamageInfo&, Damage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENWEAPONS_API ULimenDamageComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FLimenDamageEvent OnDamageReceived;

	ULimenDamageComponent();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void ApplyDamage(AController* Instigator, AActor* Causer, const TSubclassOf<ULimenDamageType>& DamageType, const FDamageParameters& DamageParams);
	template<typename T>
	void SetDamageCalculationFunction(T* Object,TMemFunPtrType<true, T, float(const FDamageParameters&, const ULimenDamageType*)>::Type Callback);
	void SetDamageCalculationFunction(const TFunction<float(const FDamageParameters&, const ULimenDamageType*)>& InFunction);

protected:

private:
	TArray<FDamageInfo> ActiveDamageInfo;
	TFunction<float(const FDamageParameters&, const ULimenDamageType*)> DamageCalcFunc;
	TArray<TStrongObjectPtr<ULimenDamageType>> DamageInstances;

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_BroadcastDamageReceived(const FDamageInfo& Info);
};

template <typename T>
void ULimenDamageComponent::SetDamageCalculationFunction(T* Object,
	typename TMemFunPtrType<true, T, float(const FDamageParameters&, const ULimenDamageType*)>::Type Callback)
{
	DamageCalcFunc = [=] (const FDamageParameters& Params, const ULimenDamageType* Type)
	{
		if (Object != nullptr) return (Object->*Callback)(Params, Type);

		return Params.DamageValue;
	};
}
