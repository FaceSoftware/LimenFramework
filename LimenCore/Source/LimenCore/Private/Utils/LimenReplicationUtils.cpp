// Copyright FaceSoftware. All Rights Reserved.


#include "Utils/LimenReplicationUtils.h"


bool FLimenReplicationUtils::ShouldPerformNetworkPrediction(const AActor* InActor)
{
	return InActor && InActor->GetNetMode() == NM_Client;
}

bool FLimenReplicationUtils::IsComponentLocalClientCopy(const UActorComponent* InComponent)
{
	if (!InComponent) { return false; }

	if (const auto* Pawn = InComponent->GetOwner<APawn>())
	{
		if (Pawn->IsLocallyControlled()) return true;
	}
	else if (const auto* PC = InComponent->GetOwner<APlayerController>())
	{
		if (PC->IsLocalController()) return true;
	}

	return false;
}
