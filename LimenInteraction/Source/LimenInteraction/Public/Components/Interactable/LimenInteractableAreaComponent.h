// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "Interfaces/LimenInteractableComponent.h"
#include "LimenInteractableAreaComponent.generated.h"


USTRUCT()
struct FInteractionParams
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AController> Controller;
	UPROPERTY()
	TObjectPtr<APawn> Pawn;
	UPROPERTY()
	bool bIsBeingInteracted;
};

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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
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

	virtual bool AllowPhysicsInteraction() const override;
	virtual bool IsBeingInteracted() const override;

	#pragma endregion ILimenInteractableComponent
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Area")
	bool bAllowPhysicsInteraction;

private:
	TOptional<FCollisionResponseContainer> CachedCollisionResponse;
	TOptional<ECollisionEnabled::Type> CachedCollisionEnabledType;

	UPROPERTY(ReplicatedUsing=OnRep_IsBeingInteracted)
	FInteractionParams InteractionParams;

	UFUNCTION()
	void OnRep_IsBeingInteracted();
};
