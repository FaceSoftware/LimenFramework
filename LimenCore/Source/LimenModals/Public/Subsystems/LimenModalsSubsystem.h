// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "UMG/LimenGenericModalWidget.h"
#include "LimenModalsSubsystem.generated.h"


class ULimenGenericModalWidget;

/**
 * 
 */
UCLASS()
class LIMENMODALS_API ULimenModalsSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	ULimenGenericModalWidget* DisplayConfirmationModal(const FModalParams& InParams);
	ULimenGenericModalWidget* DisplayConsentModal(const FModalParams& InParams);
	ULimenGenericModalWidget* DisplayTimedModal(const FModalParams& InParams);

private:
	ULimenGenericModalWidget* DisplayModalInternal(const TSubclassOf<ULimenGenericModalWidget>& ModalClass, const FModalParams& InParams) const;
};



/// Modals Blueprint Async Actions

UCLASS()
class LIMENMODALS_API UConfirmationModalAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FModalDelegate OnModalDismissed;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Modals", meta=(WorldContext="WorldContextObject"))
	static UConfirmationModalAsyncAction* DisplayConfirmationModal(const UObject* WorldContextObject, const FModalParams& InParams);

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UWorld> World;
	FModalParams Params;

	UFUNCTION()
	void ModalDismissed(ULimenGenericModalWidget* ModalWidget, const bool bAccepted);
};

UCLASS()
class LIMENMODALS_API UConsentModalAsyncAction : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FModalDelegate OnModalDismissed;
	
	UFUNCTION(BlueprintCallable, Category="Limen|Modals", meta=(WorldContext="WorldContextObject"))
	static UConsentModalAsyncAction* DisplayConsentModal(const UObject* WorldContextObject, const FModalParams& InParams);

	virtual void Activate() override;

private:
	UPROPERTY()
	TObjectPtr<UWorld> World;
	FModalParams Params;

	UFUNCTION()
	void ModalDismissed(ULimenGenericModalWidget* ModalWidget, const bool bAccepted);
};
