// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Items/LimenNonDiscardableItem.h"
#include "LimenNote.generated.h"

class ULimenNoteDataAsset;

UCLASS()
class LIMENNOTES_API ALimenNote : public ALimenNonDiscardableItem
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Limen|Interaction|Notes", BlueprintPure)
	static FText GetNoteText(const TSubclassOf<ALimenNote>& Class);

	explicit ALimenNote(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	const FText& GetNoteText() const;
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Limen")
	FText NoteText;
	
private:
	
};
