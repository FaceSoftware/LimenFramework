// Copyright FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetInteractionComponent.h"
#include "LimenWidgetInteractionComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENCORE_API ULimenWidgetInteractionComponent : public UWidgetInteractionComponent
{
	GENERATED_BODY()

public:
	ULimenWidgetInteractionComponent();
	virtual void Activate(bool bReset = false) override;
	virtual void Deactivate() override;

	virtual void PressPointerKey(FKey Key) override;

protected:

private:
};
