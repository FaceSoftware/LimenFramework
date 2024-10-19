// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenInteractionComponent.generated.h"


class ALimenInteractable;
class ALimenItemBase;
class ULimenInteractableComponent;
class ILimenInteractableComponent;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FInteractableHover, AActor*, InteractableActor, const TScriptInterface<ILimenInteractableComponent>&, Interactable);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FBPInteractionDelegate, AActor*, InteractableActor, const TScriptInterface<ILimenInteractableComponent>&, Interactable);

UCLASS(Abstract)
class LIMENINTERACTION_API ULimenInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	/**
	 * @brief Broadcasts when the user stops hovering or starts hovering an interactable
	 */
	UPROPERTY(BlueprintAssignable)
	FInteractableHover OnInteractableHover;
	UPROPERTY(BlueprintAssignable)
	FInteractableHover OnInteract;

	explicit ULimenInteractionComponent(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void OnComponentCreated() override;
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable, Category="Limen|Interaction")
	virtual void RestartInteraction() { PURE_VIRTUAL(RestartInteraction); }

	bool Interact(AController* InController, APawn* InPawn);
	bool Interact(AController* InController, APawn* InPawn, AActor* SpecificInteractable);
	bool Interact(AController* InController, APawn* InPawn, UActorComponent* SpecificInteractableComponent);

	void StopInteraction(AController* InController, APawn* InPawn);

protected:
	TScriptInterface<ILimenInteractableComponent> CurrentInteractableInterface;
	
	bool GetOwnerLineOfSightStartAndEndVectors(FVector& Start, FVector& End) const;
	
	virtual void SetupInteraction() {} // Pure virtual
	virtual void UpdateInteraction() {} // Pure virtual

	float GetInteractionRange() const;
	bool DebugMode() const;
	
	static bool IsInteractableActor(const AActor* InActor);
	static bool IsInteractableComponent(const UActorComponent* InComponent);

private:
	UPROPERTY(EditAnywhere, Category="Limen")
	float InteractionRange;

#if WITH_EDITORONLY_DATA
	UPROPERTY(EditAnywhere, Category="Limen")
	bool bDebugMode;
#endif
	
	TScriptInterface<ILimenInteractableComponent> PreviousInteractableInterface;

	bool bIsInteracting;
};
