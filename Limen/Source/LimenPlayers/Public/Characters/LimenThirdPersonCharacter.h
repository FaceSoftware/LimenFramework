// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenPlayerCharacter.h"
#include "LimenThirdPersonCharacter.generated.h"

class USpringArmComponent;

UCLASS(Abstract, Blueprintable, BlueprintType)
class LIMENPLAYERS_API ALimenThirdPersonCharacter : public ALimenPlayerCharacter
{
	GENERATED_BODY()

public:
	explicit ALimenThirdPersonCharacter(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());

protected:

private:

};
