// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class LIMENCORE_API FLimenReplicationUtils
{
public:
	template<typename T>
	static bool ShouldCallRPC(T* InObject)
	{
		if (!InObject) return false;

		AActor* Owner = nullptr;
		if constexpr (TIsDerivedFrom<T, AActor>::Value)
		{
			Owner = InObject;
		}
		else if constexpr (TIsDerivedFrom<T, UActorComponent>::Value)
		{
			Owner = InObject->GetOwner();
		}
		if (!Owner) { return false; }

		return Owner->GetNetMode() == NM_Client;
	}

	FORCEINLINE static bool ShouldPerformNetworkPrediction(const AActor* InActor)
	{
		return InActor && InActor->GetNetMode() == NM_Client;
	}
};

#define SHOULD_PREDICT_NETWORK_EVENT DELETE ME \

#define SHOULD_PREDICT_NETWORK_EVENT_COMP DELETE ME \

/**
 * @brief This will always execute the AgnosticFunction.
 * The ServerRPCFunction will only execute on NM_Client.
 * @param AgnosticFunction The actual functionality to predict.
 * @param ServerRPCFunction The RPC wrapper for the AgnosticFunction
 */
#define NETWORK_PREDICTION(AgnosticFunction, ServerRPCFunction) \
	AgnosticFunction; \
	if (FLimenReplicationUtils::ShouldCallRPC(this)) \
	{ \
		ServerRPCFunction; \
	} \
