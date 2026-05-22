// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayActor.h"
#include "Interfaces/LimenSaveObjectInterface.h"
#include "LimenInteractable.generated.h"


class ULimenInteractableComponent;

UCLASS(BlueprintType, Blueprintable)
class LIMENINTERACTION_API ALimenInteractable : public ALimenGameplayActor, public ILimenSaveObjectInterface
{
	GENERATED_BODY()

public:
	DECLARE_MULTICAST_DELEGATE_TwoParams(FInteractableDelegate, AController* /* Controller */, APawn* /* Pawn */);
	
	FInteractableDelegate OnInteract;
	FInteractableDelegate OnInteractionStopped;
	
	explicit ALimenInteractable(const FObjectInitializer& InObjectInitializer = FObjectInitializer::Get());
	virtual void PostInitializeComponents() override;

	template<typename ComponentClass = ULimenInteractableComponent>
	TArray<ComponentClass*> GetInteractableComponents() const
	{
		TArray<ULimenInteractableComponent*> InteractableComponents;
		GetComponents<ULimenInteractableComponent>(InteractableComponents);
		if (std::is_same_v<ULimenInteractableComponent, ComponentClass>)
		{
			return InteractableComponents;
		}

		TArray<ComponentClass*> OutComponents;
		OutComponents.Reserve(InteractableComponents.Num());
		
		for (auto* InteractableComponent : InteractableComponents)
		{
			OutComponents.Push(Cast<ComponentClass>(InteractableComponent));
		}
		return OutComponents;
	}
	TArray<ULimenInteractableComponent*> GetInteractableComponents() const;

	bool IsBeingContinuouslyInteracted() const;
	bool HasBeenInteracted() const;

	virtual bool ShouldSaveData() const override;
	virtual bool ShouldLoadData() const override;
	virtual void PreDataSaved() override;
	virtual void PostDataSaved() override;
	virtual void PreDataLoaded() override;
	virtual void PostDataLoaded() override;
	virtual FName GetUniqueDeterministicId() const override;
	void SetUniqueName(const FName& NewUniqueName);
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Interactable")
	FName UniqueName;

	virtual void Interact(AController* InController, APawn* InPawn);
	virtual void InteractionStopped(AController* InController, APawn* InPawn);
	
private:
	UPROPERTY(SaveGame)
	bool bWasInteracted;
	
	bool bIsBeingContinuouslyInteracted;
	
	void OnInteract_Internal(AController* InController, APawn* InPawn);
	void OnInteractionStopped_Internal(AController* InController, APawn* InPawn);
};
