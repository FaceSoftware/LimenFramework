// Copyright FaceSoftware. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LimenGameModeBase.h"
#include "LimenLobbyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class LIMENGAMEFRAMEWORK_API ALimenLobbyGameMode : public ALimenGameModeBase
{
	GENERATED_BODY()

public:
	virtual void OnPostLogin(AController* NewPlayer) override;
};
