// Copyright Face Software. All Rights Reserved.


#include "GameInstances/LimenGameInstance.h"

#if !UE_BUILD_SHIPPING

#include "BlueprintLibraries/LimenCoreStatics.h"

#endif

ULimenGameInstance* ULimenGameInstance::GetGameInstance(UWorld* World)
{
	return GetGameInstance<ULimenGameInstance>(World);
}