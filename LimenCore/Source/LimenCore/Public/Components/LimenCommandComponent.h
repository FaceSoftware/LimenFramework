// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenCommandComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCommandRequest, const FName&, Command, const TArray<FName>&, Args, UObject*, Requestor);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenCommandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FCommandRequest OnCommandRequested;

	ULimenCommandComponent();

	UFUNCTION(BLueprintCallable)
	void ExecuteCommand(const FName& Command, const TArray<FName>& Args, UObject* Requestor);
	void ExecuteCommand(const FName& Command, const FName& Arg, UObject* Requestor);

protected:	
	
private:
};
