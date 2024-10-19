// Copyright Face Software. All Rights Reserved.


#include "Items/LimenNote.h"



FText ALimenNote::GetNoteText(const TSubclassOf<ALimenNote>& Class)
{
	return Class->GetDefaultObject<ALimenNote>()->GetNoteText();
}

const FText& ALimenNote::GetNoteText() const
{
	return NoteText;
}
