// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenDynamicLoadingComponent.generated.h"


class UCameraComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCEDURALMAPS_API ULimenDynamicLoadingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	ULimenDynamicLoadingComponent();
	virtual void BeginPlay() override;

	UFUNCTION()
	void UpdateLoadState();

	UFUNCTION(BlueprintCallable, Category="Limen|Procedural Generation|Dynamic Loading")
	bool IsLoaded();

	void SetTileUnloadDistance(const float NewUnloadDistance);

protected:	
	virtual void LoadActor();
	virtual void UnloadActor();

	/**
	 * @brief Set rules for whether the actor should be loaded or unloaded.
	 * @return True if it should be loaded, false if it should  be unloaded.
	 */
	UFUNCTION()
	virtual bool ShouldLoad();

private:
	FTimerHandle LoadCheckTimer;
	uint8 bIsLoaded : 1;
	float TileUnloadDistance;
	
	FCriticalSection LoadedSection;

	void LoadHandler();
};
