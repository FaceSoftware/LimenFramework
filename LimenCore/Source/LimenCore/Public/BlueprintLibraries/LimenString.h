// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LimenString.generated.h"


/**
 * 
 */
UCLASS()
class LIMENCORE_API ULimenString : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * @brief Splits a string at a specific index. If 1234x5678 and the index is 4, LeftPart = 1234; and RightPart = 5678
	 * @param InString The string to split
	 * @param Index The index of the cutting point (the character at this index will be removed entirely from the left and right parts)
	 * @param OutLeftPart The left part of the string
	 * @param OutRightPart The right part of the string
	 */
	UFUNCTION(BlueprintCallable, Category="Limen String Library")
	static void SplitStringAtIndex(const FString& InString, const int32 Index, FString& OutLeftPart, FString& OutRightPart);

	UFUNCTION(BlueprintCallable, Category="Limen String Library")
	static bool CompareText(const FText& A, const FText& B);
};
