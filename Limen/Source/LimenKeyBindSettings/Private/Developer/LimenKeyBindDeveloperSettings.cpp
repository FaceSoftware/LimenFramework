// Copyright © 2024 FaceSoftware. All rights reserved.


#include "Developer/LimenKeyBindDeveloperSettings.h"


TArray<TSoftObjectPtr<UInputMappingContext>> ULimenKeyBindDeveloperSettings::GetAllInputMappingContexts() const
{
	TArray<FMappingContextSoftPtr> PlayerMappings;
	PlayerMappingContexts.GenerateValueArray(PlayerMappings);
	
	TArray<FMappingContextSoftPtr> PawnMappings;
	PawnMappingContexts.GenerateValueArray(PawnMappings);

	PawnMappings.Append(PlayerMappings);

	return PawnMappings;
}
