// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamageTypes/LimenDamageType.h"
#include "LimenDamageComponent.generated.h"

class ULimenDamageType;
class AController;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FLimenDamageEvent, AController*, Instigator,
	AActor*, Causer, TSubclassOf<ULimenDamageType>, DamageType, const float, Damage);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENWEAPONS_API ULimenDamageComponent : public UActorComponent
{
	GENERATED_BODY()

	struct FDamageInfo
	{
		TWeakObjectPtr<AController> Instigator;
		TWeakObjectPtr<AActor> Causer;
		TStrongObjectPtr<ULimenDamageType> DamageType;
		FDamageParameters DamageParameters;
	};

public:
	FLimenDamageEvent OnDamageReceived;

	ULimenDamageComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void ApplyDamage(AController* Instigator, AActor* Causer, const TSubclassOf<ULimenDamageType>& DamageType,
		const FDamageParameters& DamageParams);

	template<typename T>
	void SetDamageCalculationFunction(T* Object,
		typename TMemFunPtrType<true, T, float(const FDamageParameters&, const TSubclassOf<ULimenDamageType>&)>::Type Callback);

protected:

private:
	TArray<FDamageInfo> ActiveDamageInfo;
	TFunction<float(const FDamageParameters&, const TSubclassOf<ULimenDamageType>&)> DamageCalcFunc;

};

template <typename T>
void ULimenDamageComponent::SetDamageCalculationFunction(T* Object,
	typename TMemFunPtrType<true, T, float(const FDamageParameters&, const TSubclassOf<ULimenDamageType>&)>::Type Callback)
{
	DamageCalcFunc = [=] (const FDamageParameters& Params, const TSubclassOf<ULimenDamageType>& Type)
	{
		if (Object != nullptr) return (Object->*Callback)(Params, Type);

		return Params.DamageValue;
	};
}
