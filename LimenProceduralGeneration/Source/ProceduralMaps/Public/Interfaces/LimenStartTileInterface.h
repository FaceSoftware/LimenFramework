// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenStartTileInterface.generated.h"

class AController;
// This class does not need to be modified.
UINTERFACE()
class PROCEDURALMAPS_API ULimenStartTileInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class PROCEDURALMAPS_API ILimenStartTileInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetSpawnTransform() const;

	UFUNCTION(BlueprintImplementableEvent)
	void PlayerTeleported(AController* Controller, APawn* Pawn);

protected:
	virtual FVector GetDefaultSpawnLocation_Implementation() const { return FVector(0); }
	
private:
	
};
