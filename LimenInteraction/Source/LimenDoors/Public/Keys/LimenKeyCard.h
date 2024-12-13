// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenNonDiscardableItem.h"
#include "LimenKeyCard.generated.h"

UCLASS()
class LIMENDOORS_API ALimenKeyCard : public ALimenNonDiscardableItem
{
	GENERATED_BODY()

public:
	explicit ALimenKeyCard(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	int GetKeyCardLevel() const;
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Limen")
	int32 KeyCardLevel;

private:
	
};
