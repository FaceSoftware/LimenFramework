// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenPuzzleElementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPuzzleElementStateDelegate);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENMODULARPUZZLES_API ULimenPuzzleElementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenPuzzleElementComponent();
	virtual void BeginPlay() override;

	virtual const FName& GetElementId() const;

protected:
	UPROPERTY(EditAnywhere)
	FName ElementId;

private:
};
