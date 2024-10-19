// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenWidget.h"
#include "LimenGenericModalWidget.generated.h"


USTRUCT(BlueprintType)
struct FModalParams
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Title = FText::FromString(TEXT("ModalTile"));
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Content = FText::FromString(TEXT("ModalContent"));;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FModalDelegate, bool, bAccepted);
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

	UFUNCTION(BlueprintCallable, Category="Limen|Modals")
	void ModalDismissed(const bool bAccepted);
};
