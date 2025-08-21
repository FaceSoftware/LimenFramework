// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenPuzzleElementComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENMODULARPUZZLES_API ULimenPuzzleElementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenPuzzleElementComponent();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	const FName& GetElementId() const;
	UFUNCTION(BlueprintCallable)
	void SetElementId(const FName& NewId);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ElementId;

private:
};
