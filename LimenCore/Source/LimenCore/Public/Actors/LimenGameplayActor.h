// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/LimenCoreObjectInterface.h"
#include "LimenGameplayActor.generated.h"

class ULimenObjectiveComponent;
class ULimenCollectableComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FObjectInitializationDelegate, UObject*, Actor);

/**
 * Base class for gameplay actors.
 */
UCLASS(Abstract, NotBlueprintable)
class LIMENCORE_API ALimenGameplayActor : public AActor, public ILimenCoreObjectInterface
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FObjectInitializationDelegate OnActorInitialized;

	explicit ALimenGameplayActor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;
	virtual void Initialize() override;
	
	/**
	 * @brief Disables this actor's collision and visibility
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay Actor")
	void RemoveFromGameplay();
	/**
	 * @brief Disables this actor's collision and visibility
	 */
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay Actor")
	void AddToGameplay();
	UFUNCTION(BlueprintCallable, Category="Limen|Gameplay Actor")
	bool IsRemovedFromGameplay() const;

protected:
	
private:
	bool bIsRemovedFromGameplay;
	
	void Initialize_Internal();
};