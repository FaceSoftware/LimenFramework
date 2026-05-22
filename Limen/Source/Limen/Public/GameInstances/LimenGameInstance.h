// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ShaderPipelineCache.h"
#include "Engine/GameInstance.h"
#include "Engine/World.h"
#include "LimenGameInstance.generated.h"


/**
 * 
 */
UCLASS()
class LIMEN_API ULimenGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:

	template<typename GameInstanceClass>
	static GameInstanceClass* GetGameInstance(UWorld* World)
	{
		static_assert(std::is_base_of_v<UGameInstance, GameInstanceClass>);
		GameInstanceClass* GameInstancePtr = Cast<GameInstanceClass>(World->GetGameInstance());
		
		return IsValid(GameInstancePtr) ? GameInstancePtr : nullptr;
	}

	static ULimenGameInstance* GetGameInstance(UWorld* World);

protected:

private:
	
};
