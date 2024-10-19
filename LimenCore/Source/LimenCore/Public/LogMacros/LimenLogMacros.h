// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLimenCore, Log, All);
inline DEFINE_LOG_CATEGORY(LogLimenCore);
DECLARE_LOG_CATEGORY_EXTERN(LogLimen, Log, All);
inline DEFINE_LOG_CATEGORY(LogLimen);


/* Log macro with verbosity. Style: ClassName, Message. */
#define LIMEN_LOG(CategoryName, Verbosity, CallingObject, FormatString , ...) \
	UE_LOG(CategoryName, Verbosity, TEXT("Instance %s(%s): %s"), \
	*CallingObject->GetName(), \
	*CallingObject->GetClass()->GetName(), \
	*FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) ) \


#define STATIC_LIMEN_LOG(CategoryName, Verbosity, CallingFunctionString, FormatString , ...) \
	UE_LOG(CategoryName, Verbosity, TEXT("Static Log (%s): %s"), \
	*CallingFunctionString, \
	*FString::Printf(TEXT(FormatString), ##__VA_ARGS__ ) ) \
