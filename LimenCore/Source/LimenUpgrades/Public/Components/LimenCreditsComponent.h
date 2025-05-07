// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenCreditsComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCreditsUpdate, class ULimenCreditsComponent*, CreditsComponent, const int32, NewCredits);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENUPGRADES_API ULimenCreditsComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCreditsUpdate OnCreditsUpdated;

	ULimenCreditsComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void DepositCredits(const int32 NewCredits);
	UFUNCTION(BlueprintCallable, Category="Credits")
	bool WithdrawCredits(const int32 OutCredits);
	int32 GetCredits() const;
	void SetStartingCredits(const int32 NewStartingCredits);

protected:
	UPROPERTY(EditDefaultsOnly, Category="Credits", BlueprintReadOnly)
	bool bAllowNegativeBalance;
	UPROPERTY(EditDefaultsOnly, Category="Credits", BlueprintReadOnly)
	int32 StartingCredits;

	virtual void CreditsUpdated();

private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentCredits, Transient, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 CurrentCredits;

	UFUNCTION()
	void OnRep_CurrentCredits();
};
