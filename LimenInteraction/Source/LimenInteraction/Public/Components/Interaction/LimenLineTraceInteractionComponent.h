// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "CollisionQueryParams.h"
#include "LimenInteractionComponent.h"
#include "LimenLineTraceInteractionComponent.generated.h"


struct FHitResult;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenLineTraceInteractionComponent : public ULimenInteractionComponent
{
	GENERATED_BODY()

public:
	explicit ULimenLineTraceInteractionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	ECollisionChannel GetTraceChannel() const;
	
protected:
	UPROPERTY(EditAnywhere, Category="Limen")
	TEnumAsByte<ECollisionChannel> InteractionCollisionChannel;

	virtual void SetupInteraction() override;
	virtual void UpdateInteraction(const float DeltaTime) override final;

private:
	TArray<FHitResult> InteractionResults;

	FCollisionQueryParams QueryParams;

	static FCollisionResponseParams CollisionResponseParams;
	static FCollisionQueryParams CollisionQueryParams;
};
