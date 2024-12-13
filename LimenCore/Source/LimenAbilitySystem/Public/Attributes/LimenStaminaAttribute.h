// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenAttributeBase.h"
#include "LimenStaminaAttribute.generated.h"

/**
 * 
 */
UCLASS()
class LIMENABILITYSYSTEM_API ULimenStaminaAttribute : public ULimenAttributeBase
{
	GENERATED_BODY()

public:
	ULimenStaminaAttribute();
	virtual void Initialize(AActor* InOwner) override;

protected:


private:
};
