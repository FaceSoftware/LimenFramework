// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenInteractablePromptComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENINTERACTION_API ULimenInteractablePromptComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FInteractablePrompt, const FText&, Prompt);
	UPROPERTY(BlueprintAssignable)
	FInteractablePrompt OnPromptChanged;	

	ULimenInteractablePromptComponent();

	const FText& GetPromptText() const;
	UFUNCTION(BlueprintCallable)
	void SetPromptText(const FText& NewPrompt);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText InteractionPrompt;

private:
};
