// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LimenUpgrade.generated.h"

class ULimenCreditsComponent;
class UCurveFloat;
class AController;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpgradeApplied, class ULimenUpgrade*, Upgrade);

UCLASS(BlueprintType, Blueprintable)
class LIMENUPGRADES_API ULimenUpgrade : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FUpgradeApplied OnUpgradeApplied;
	
	ULimenUpgrade();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override;
	virtual int32 GetFunctionCallspace(UFunction* Function, FFrame* Stack) override;
	virtual bool CallRemoteFunction(UFunction* Function, void* Parameters, FOutParmRec* OutParms, FFrame* Stack) override;


	UFUNCTION(BlueprintCallable, Category="Upgrade")
	void ProcessUpgrade(AController* InController, APawn* InPawn, ULimenCreditsComponent* CreditsComponent);
	UFUNCTION(BlueprintCallable, Category="Upgrade")
	virtual bool CanApplyUpgrade(AController* InController, APawn* InPawn, const ULimenCreditsComponent* CreditsComponent) const;
	UFUNCTION(BlueprintCallable, Category="Upgrade")
	int32 GetCurrentUpgradeLevel() const;

	UFUNCTION(BlueprintCallable, Category="Upgrade")
	virtual int32 GetCost(int32 Level) const;
	UFUNCTION(BlueprintCallable, Category="Upgrade")
	virtual float GetUpgradeValue(int32 Level) const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Upgrade", BlueprintReadOnly)
	FText UpgradeName;
	UPROPERTY(EditDefaultsOnly, Category="Upgrade", BlueprintReadOnly)
	FText UpgradeDescription;
	UPROPERTY(EditDefaultsOnly, Category="Upgrade", BlueprintReadOnly, meta=(ClampMin="0"))
	int32 MaxLevel;
	UPROPERTY(EditDefaultsOnly, Category="Upgrade", BlueprintReadOnly, meta=(ClampMin="0"))
	TObjectPtr<UCurveFloat> CostFunction;
	UPROPERTY(EditDefaultsOnly, Category="Upgrade", BlueprintReadOnly, meta=(ClampMin="0"))
	TObjectPtr<UCurveFloat> UpgradeFunction;

	virtual void ProcessUpgradeInternal(AController* InController, APawn* InPawn, float NewUpgradeValue);
	virtual void CurrentLevelChanged();

private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentLevel, BlueprintReadOnly, meta=(AllowPrivateAccess=true))
	int32 CurrentLevel;
	UPROPERTY(Replicated)
	TWeakObjectPtr<AController> BoundController;
	UPROPERTY(Replicated)
	TWeakObjectPtr<APawn> BoundPawn;

	UFUNCTION()
	void OnRep_CurrentLevel();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ProcessUpgrade(AController* InController, APawn* InPawn, float NewUpgradeValue);
};
