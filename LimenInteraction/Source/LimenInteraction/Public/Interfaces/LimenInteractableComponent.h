// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LimenInteractableComponent.generated.h"


// This class does not need to be modified.
UINTERFACE(Blueprintable, BlueprintType)
class LIMENINTERACTION_API ULimenInteractableComponent : public UInterface
{
	GENERATED_BODY()
};


DECLARE_MULTICAST_DELEGATE_TwoParams(FInteractableComponentDelegate, AController* /* InteractionController */, APawn* /* InteractionPawn */);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBPInteractableComponentDelegate, AController*, InteractionController, APawn*, InteractionPawn);


/**
 * 
 */
class LIMENINTERACTION_API ILimenInteractableComponent
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual FInteractableComponentDelegate* GetInteractionDelegate() = 0;
	virtual FInteractableComponentDelegate* GetInteractionStoppedDelegate() = 0;
	virtual FBPInteractableComponentDelegate* GetBlueprintInteractionDelegate() = 0;
	virtual FBPInteractableComponentDelegate* GetBlueprintInteractionStoppedDelegate() = 0;
	virtual UPrimitiveComponent* GetPrimitiveComponent() = 0;
	template<typename T>
	T* GetPrimitiveComponent()
	{
		static_assert(TIsDerivedFrom<T, UPrimitiveComponent>::Value);
		return Cast<T>(GetPrimitiveComponent());
	}

	virtual void NotifyHover(AController* Controller, APawn* Pawn) = 0;
	virtual void NotifyUnHover(AController* Controller, APawn* Pawn) = 0;
	virtual void Interact(AController* InController, APawn* InPawn) = 0;
	virtual void StopInteraction(AController* InController, APawn* InPawn) = 0;

	virtual bool AllowPhysicsInteraction() const = 0;
	virtual bool IsBeingInteracted() const = 0;
};
