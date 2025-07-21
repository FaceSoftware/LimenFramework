// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interfaces/LimenInteractableComponent.h"
#include "LimenInteractableAreaComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenInteractableAreaComponent : public UBoxComponent, public ILimenInteractableComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, DisplayName="On Interact", Category="Limen")
	FBPInteractableComponentDelegate BP_OnInteract;
	UPROPERTY(BlueprintAssignable, DisplayName="On Hover Start", Category="Limen")
	FBPInteractableComponentDelegate BP_OnHoverStart;
	FInteractableComponentDelegate OnInteract;
	UPROPERTY(BlueprintAssignable, DisplayName="On Interaction Stopped", Category="Limen")
	FBPInteractableComponentDelegate BP_OnInteractionStopped;
	UPROPERTY(BlueprintAssignable, DisplayName="On Hover End", Category="Limen")
	FBPInteractableComponentDelegate BP_OnHoverEnd;
	FInteractableComponentDelegate OnInteractionStopped;
	
	ULimenInteractableAreaComponent();
	virtual void OnComponentCreated() override;
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;


#pragma region ILimenInteractableComponent

	virtual FInteractableComponentDelegate* GetInteractionDelegate() override;
	virtual FInteractableComponentDelegate* GetInteractionStoppedDelegate() override;
	virtual FBPInteractableComponentDelegate* GetBlueprintInteractionDelegate() override;
	virtual FBPInteractableComponentDelegate* GetBlueprintInteractionStoppedDelegate() override;
	virtual UPrimitiveComponent* GetPrimitiveComponent() override;
	virtual void Interact(AController* InController, APawn* InPawn) override;
	virtual void StopInteraction(AController* InController, APawn* InPawn) override;
	virtual void NotifyHover(AController* Controller, APawn* Pawn) override;
	virtual void NotifyUnHover(AController* Controller, APawn* Pawn) override;

	#pragma endregion ILimenInteractableComponent
protected:

private:
	TOptional<FCollisionResponseContainer> CachedCollisionResponse;
	TOptional<ECollisionEnabled::Type> CachedCollisionEnabledType;
};
