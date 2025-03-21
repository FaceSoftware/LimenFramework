// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Interactables/LimenInteractable.h"
#include "LimenArchiveItem.generated.h"


class ULimenArchive;

UCLASS()
class LIMENARCHIVES_API ALimenArchiveItem : public ALimenInteractable
{
	GENERATED_BODY()

public:
	explicit ALimenArchiveItem(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category="Limen|Archives", meta=(ExpandBoolAsExecs="ReturnValue"))
	bool HasAlreadyBeenArchived() const;

protected:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<ULimenArchive> BoundArchiveClass;
	UPROPERTY(EditDefaultsOnly)
	float InteractAnimationTime;

	virtual void Interact(AController* InController, APawn* InPawn) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCosmetic)
	void InteractAnimation(const float AnimationTime);

private:
	TWeakObjectPtr<ULimenArchive> ArchivePtr;
	FTimerHandle InteractAnimationTimerHandle;
	
};
