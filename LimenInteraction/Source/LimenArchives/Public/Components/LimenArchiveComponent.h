// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "LimenArchiveComponent.generated.h"


class ULimenArchiveDisplayWidget;
class ULimenArchive;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LIMENARCHIVES_API ULimenArchiveComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	ULimenArchiveComponent();
	
	UFUNCTION(BlueprintCallable, Category="Limen|Archives", meta=(ExpandBoolAsExecs="ReturnValue"))
	bool HasAlreadyBeenArchived() const;

	UFUNCTION(BlueprintCallable, Category="Limen|Archives")
	ULimenArchive* GetArchive() const;

protected:
	UPROPERTY(EditAnywhere)
	TSoftClassPtr<ULimenArchive> BoundArchiveClass;

private:
	
};
