// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenKeyCardReader.generated.h"


class ALimenKeyCard;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENDOORS_API ULimenKeyCardReader : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenKeyCardReader(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	bool ReadKeyCard(ALimenKeyCard* KeyCard);

	UFUNCTION(BlueprintCallable)
	int GetMinimumKeyCardLevel() const;
	UFUNCTION(BlueprintCallable)
	void SetMinimumKeyCardLevel(const int NewMinLevel);
	
protected:

private:
	UPROPERTY(EditDefaultsOnly)
	uint16 MinKeyCardLevel;
};
