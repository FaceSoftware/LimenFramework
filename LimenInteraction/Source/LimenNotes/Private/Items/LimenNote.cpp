// Copyright Face Software. All Rights Reserved.


#include "Items/LimenNote.h"



FText ALimenNote::GetNoteText(const TSubclassOf<ALimenNote>& Class)
{
	return Class->GetDefaultObject<ALimenNote>()->GetNoteText();
}

ALimenNote::ALimenNote(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.DoNotCreateDefaultSubobject(TEXT("ItemImageSceneCapture")))
{
}

const FText& ALimenNote::GetNoteText() const
{
	return NoteText;
}
