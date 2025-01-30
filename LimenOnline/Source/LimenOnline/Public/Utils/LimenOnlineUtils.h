// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class LIMENONLINE_API FLimenOnlineUtils
{
public:
	[[nodiscard]] static FString Encrypt(const TArray<uint8>& Data, const FString& Key);
	[[nodiscard]] static TArray<uint8> Decrypt(const FString& Data, const FString& Key);
};
