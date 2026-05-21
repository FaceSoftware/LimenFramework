// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "LimenInteractableComponent.generated.h"


USTRUCT()
struct FInteractionParams
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<AController> Controller;
	UPROPERTY()
	TObjectPtr<APawn> Pawn;
	UPROPERTY()
	bool bIsBeingInteracted = false;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenInteractableComponent : public UBoxComponent
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FInteractableComponentDelegate, AController* /* InteractionController */, APawn* /* InteractionPawn */);
	
	FInteractableComponentDelegate OnInteract;
	FInteractableComponentDelegate OnInteractionStopped;
	FInteractableComponentDelegate OnHoverStart;
	FInteractableComponentDelegate OnHoverEnd;
	
	ULimenInteractableComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void InitializeComponent() override;
	virtual void Activate(bool bReset) override;
	virtual void Deactivate() override;
	
	virtual void Interact(AController* InController, APawn* InPawn);
	virtual void StopInteraction(AController* InController, APawn* InPawn);
	virtual void NotifyHover(AController* Controller, APawn* Pawn);
	virtual void NotifyUnHover(AController* Controller, APawn* Pawn);

	virtual bool AllowPhysicsInteraction() const;
	virtual bool IsBeingInteracted() const;
	
	const FGuid& GetReplicatedGuid() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interactable Area")
	bool bAllowPhysicsInteraction;

private:
	UPROPERTY(Replicated)
	FGuid ReplicatedGuid;
	
	TOptional<FCollisionResponseContainer> CachedCollisionResponse;
	TOptional<ECollisionEnabled::Type> CachedCollisionEnabledType;

	UPROPERTY(ReplicatedUsing=OnRep_IsBeingInteracted)
	FInteractionParams InteractionParams;

	UFUNCTION()
	void OnRep_IsBeingInteracted();
};
