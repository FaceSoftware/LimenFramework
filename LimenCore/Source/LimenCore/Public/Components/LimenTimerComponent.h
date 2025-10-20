// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenTimerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FLimenTimerUpdate, ULimenTimerComponent*, Component);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenTimerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	inline static const FDateTime InvalidTime = FDateTime(-1);

	UPROPERTY(BlueprintAssignable)
	FLimenTimerUpdate OnStart;
	UPROPERTY(BlueprintAssignable)
	FLimenTimerUpdate OnStop;

	ULimenTimerComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ResetTimer();
	UFUNCTION(BlueprintCallable)
	void StartTimer();
	UFUNCTION(BlueprintCallable)
	void StopTimer();
	UFUNCTION(BlueprintCallable)
	FTimespan GetCurrentTime() const;
	UFUNCTION(BlueprintCallable)
	FDateTime GetStartTime() const;
	UFUNCTION(BlueprintCallable)
	bool IsTimerActive() const;

protected:
	UFUNCTION()
	virtual void OnRep_Start();
	UFUNCTION()
	virtual void OnRep_End();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Start)
	FDateTime Start;
	UPROPERTY(ReplicatedUsing=OnRep_End)
	FDateTime End;
};
