// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenNonDiscardableItem.h"
#include "LimenKey.generated.h"

UCLASS()
class LIMENDOORS_API ALimenKey : public ALimenNonDiscardableItem
{
	GENERATED_BODY()

public:
	FName GetKeyword() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	FName Keyword;

private:
	
	
};
