// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenAttackAbilityInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable)
class LIMENABILITYSYSTEM_API ULimenAttackAbilityInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class LIMENABILITYSYSTEM_API ILimenAttackAbilityInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	virtual float GetAbilityDamage() const = 0;
	UFUNCTION(BlueprintCallable, Category="Limen|Abilities")
	virtual float GetAbilityRange() const = 0;
};
