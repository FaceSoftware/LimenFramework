// Copyright © 2024 FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"


/**
 * 
 */
class LIMENCORE_API FLimenPSOCompiler
{
public:
	enum class EContext
	{
		LoadingScreen,
		UI,
		InGame,
	};
	
	static FLimenPSOCompiler* Get();
	~FLimenPSOCompiler();
	
	void StartBatching(const EContext InContext);
	void StopBatching();
	bool IsBatchingRunning();

private:
	FLimenPSOCompiler();
};
