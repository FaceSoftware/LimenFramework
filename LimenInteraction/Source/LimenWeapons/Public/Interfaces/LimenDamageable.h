// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenDamageable.generated.h"


// This class does not need to be modified.
UINTERFACE(Blueprintable)
class LIMENWEAPONS_API ULimenDamageable : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENWEAPONS_API ILimenDamageable
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual float ApplyPointDamage(AController* DamageInstigator, AActor* DamageCauser, const float DamageTaken) = 0;
	virtual float ApplyMaxDamage(AController* DamageInstigator, AActor* DamageCauser) = 0;
};
