// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenCreditsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCreditsUpdate, class ULimenCreditsComponent*, CreditsComponent, const FString, NewCreditsString);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENUPGRADES_API ULimenCreditsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCreditsUpdate OnCreditsUpdated;

	explicit ULimenCreditsComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void DepositCredits(const int64 NewCredits);
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool WithdrawCredits(const int64 OutCredits);
	UFUNCTION(BlueprintCallable, Category="Credits")
	FString GetCreditsString() const;
	int64 GetCredits() const;
	void SetStartingCredits(const int32 NewStartingCredits);
	void SetCreditsMultiplier(const float NewMultiplier);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Credits", BlueprintReadOnly)
	bool bAllowNegativeBalance;
	UPROPERTY(EditDefaultsOnly, Category="Credits")
	uint64 StartingCredits;

	virtual void CreditsUpdated();

private:
	float Multiplier;
	UPROPERTY(ReplicatedUsing=OnRep_CurrentCredits, Transient)
	uint64 CurrentCredits;

	UFUNCTION()
	void OnRep_CurrentCredits();
};
