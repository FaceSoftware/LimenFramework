// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenInteractionComponent.h"
#include "LimenLineTraceInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenLineTraceInteractionComponent : public ULimenInteractionComponent
{
	GENERATED_BODY()

public:
	explicit ULimenLineTraceInteractionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	bool GetInteractionHitResult(FHitResult& OutHit) const;

protected:
	virtual void UpdateInteraction() override;

private:
	UPROPERTY(EditAnywhere, Category="Limen")
	TEnumAsByte<ECollisionChannel> InteractionCollisionChannel;
	// UPROPERTY(EditAnywhere, Category="Limen")
	// float LineTraceRadius;
	FCollisionQueryParams QueryParams;
	TArray<FHitResult> InteractionResults;

	TOptional<FHitResult> InteractionHitResult;

	static FCollisionResponseParams CollisionResponseParams;
	static FCollisionQueryParams CollisionQueryParams;
};
