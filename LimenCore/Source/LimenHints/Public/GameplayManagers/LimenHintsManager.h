// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Actors/LimenGameplayManager.h"
#include "LimenHintsManager.generated.h"


class ULimenHintWidget;
class ULimenHint;

UCLASS()
class LIMENHINTS_API ALimenHintsManager : public ALimenGameplayManager
{
	GENERATED_BODY()

public:
	ALimenHintsManager();
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	void SetHintWidgetClass(const TSubclassOf<ULimenHintWidget>& InClass);
	void InitializeHints();
	void EnableHints();
	void DisableHints();
	bool AreHintsEnabled() const;

	template<typename HintClass>
	HintClass* GetHint() const
	{
		static_assert(std::is_base_of_v<ULimenHint, HintClass>);
		
		for (const TStrongObjectPtr<ULimenHint>& Hint : Hints)
		{
			if (HintClass* Test = Cast<HintClass>(Hint.Get()))
			{
				return Test;
			}
		}

		return nullptr;
	}
	
protected:
	UPROPERTY(EditAnywhere, Category="Limen")
	TArray<TSoftClassPtr<ULimenHint>> HintClasses;

	TArray<TStrongObjectPtr<ULimenHint>> Hints;
	
	UPROPERTY(EditDefaultsOnly, Category="Limen")
	TSoftClassPtr<ULimenHintWidget> HintWidgetClass;
	
	bool bHintsEnabled;
};
