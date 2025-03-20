// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenWidget.h"
#include "LimenGenericModalWidget.generated.h"


USTRUCT(BlueprintType)
struct LIMENMODALS_API FModalParams
{
	GENERATED_BODY()

	FModalParams() = default;
	FModalParams(const FString& InTitle, const FString& InContent);
	FModalParams(const FText& InTitle, const FText& InContent);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title = FText::FromString(TEXT("ModalTile"));
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Content = FText::FromString(TEXT("ModalContent"));
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FModalDelegate, ULimenGenericModalWidget*, ModalWidget, bool, bAccepted);
/**
 * 
 */
UCLASS()
class LIMENMODALS_API ULimenGenericModalWidget : public ULimenWidget
{
	GENERATED_BODY()

public:
	FModalDelegate OnModalResponseReceived;

	explicit ULimenGenericModalWidget(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	void SetParams(const FModalParams& InParams);

protected:
	UPROPERTY(BlueprintReadOnly)
	FModalParams ModalParams;

	UFUNCTION(BlueprintImplementableEvent)
	void ModalParamsUpdated(const FModalParams& InModalParams);

	UFUNCTION(BlueprintCallable, Category="Limen|Modals")
	void ModalDismissed(const bool bAccepted);
};
