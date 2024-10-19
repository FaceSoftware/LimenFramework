// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Namespaces/LimenCoreMath.h"
#include "LimenConeTraceComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FActorConeUpdate, AActor*, Actor, const bool, bIsInside);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenConeTraceComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FActorConeUpdate OnConeCollisionUpdate;
	
	explicit ULimenConeTraceComponent(const FObjectInitializer& ObjectInitializer);
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void AddLineTraceActorToIgnore(AActor* Actor);
	
	UFUNCTION(BlueprintCallable, Category="Limen|Cone Trace")
	void AddActorToCheck(AActor* InActor);
	
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;

	void SetConeData(const FConeData& InConeData);

protected:
	UPROPERTY(EditDefaultsOnly)
	TEnumAsByte<ECollisionChannel> LineTraceCollisionChannel;
	
private:
	/*
	bool CreateConeMesh();
	void DestroyConeMesh();
	*/

	FConeData ConeData;

	// virtual bool IsActorInsideMesh(const AActor* Test);

	UPROPERTY()
	TMap<AActor*, bool> ActorsToCheck;

	FCollisionQueryParams LineTraceQueryParams;
	UPROPERTY()
	TArray<AActor*> IgnoredActors;

	bool IsActorInCone(const AActor* Test);
};
