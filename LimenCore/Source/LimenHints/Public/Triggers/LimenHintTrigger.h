// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Triggers/LimenTrigger.h"
#include "LimenHintTrigger.generated.h"

class ULimenHintWidget;
class ULimenHint;

UCLASS()
class LIMENHINTS_API ALimenHintTrigger : public ALimenTrigger
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<ULimenHint> HintClass;
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<ULimenHintWidget> HintWidgetClass;

	virtual void ComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
									   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
									   const FHitResult& SweepResult) override;

	template<typename T = ULimenHint>
	ULimenHint* GetHintInstance() const
	{
		if constexpr (std::is_same_v<T, ULimenHint>)
		{
			return HintInstance.Get();
		}
		else
		{
			return Cast<T>(HintInstance.Get());
		}
	}
private:
	TStrongObjectPtr<ULimenHint> HintInstance;
};
