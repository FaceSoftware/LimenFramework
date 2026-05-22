// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/LimenGameMenuWidget.h"
#include "LimenObjectiveInventory.generated.h"

class ULimenObjectiveComponent;

UCLASS(Abstract, Blueprintable)
class LIMENOBJECTIVES_API ULimenObjectiveInventory : public ULimenGameMenuWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void BindObjectiveComponent(ULimenObjectiveComponent* ObjectiveComponent);

protected:
	UFUNCTION(BlueprintCallable)
	ULimenObjectiveComponent* GetBoundObjectiveComponent() const;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnObjectiveComponentBound(ULimenObjectiveComponent* ObjectiveComponent);

private:
	UPROPERTY()
	TObjectPtr<ULimenObjectiveComponent> BoundObjectiveComponent;
};
