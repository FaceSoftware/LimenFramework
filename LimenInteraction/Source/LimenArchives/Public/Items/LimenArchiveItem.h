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

	TStrongObjectPtr<ULimenArchive> GetArchive() const;

protected:
	virtual void Interact(AController* InController, APawn* InPawn) override;

	UPROPERTY(EditAnywhere)
	TSoftClassPtr<ULimenArchive> BoundArchiveClass;

private:
	mutable TStrongObjectPtr<ULimenArchive> ArchivePtr;
	
};
