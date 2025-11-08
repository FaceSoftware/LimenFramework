// Copyright Face Software. All Rights Reserved.


#include "BlueprintLibraries/LimenString.h"


void ULimenString::SplitStringAtIndex(const FString& InString, const int32 Index, FString& OutLeftPart, FString& OutRightPart)
{
	checkf(Index >= 0 && Index < InString.Len() - 1, TEXT("Index %d out of string range"), Index)

	if (InString.IsEmpty())
	{
		return;
	}

	// Left side
	OutLeftPart.Empty();
	for (int i = 0; i < Index; ++i)
	{
		OutLeftPart += InString[i];	
	}

	// Right side
	OutRightPart.Empty();
	for (int i = Index + 1; i < InString.Len(); ++i)
	{
		OutRightPart += InString[i];	
	}
}

bool ULimenString::CompareText(const FText& A, const FText& B)
{
	return A.CompareTo(B) < 0;
}
