// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class LIMENCORE_API FLimenReplicationUtils
{
public:
	static bool ShouldPerformNetworkPrediction(const AActor* InActor);
	static bool IsComponentLocalClientCopy(const UActorComponent* InComponent);
};

#define SHOULD_PREDICT_NETWORK_EVENT \
	FLimenReplicationUtils::ShouldPerformNetworkPrediction(this) \

#define SHOULD_PREDICT_NETWORK_EVENT_COMP \
	FLimenReplicationUtils::ShouldPerformNetworkPrediction(GetOwner()) \

#define IS_COMP_LOCAL_CLIENT_COPY \
	FLimenReplicationUtils::IsComponentLocalClientCopy(this) \
