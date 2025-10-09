// Copyright Face Software. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "HAL/Runnable.h"
#include "UObject/Linker.h"
#include "DataAssets/ProceduralMapParameters.h"
#include "Maps/LimenProceduralMap.h"
#include "LimenMapAlgorithm.generated.h"


/**
 * 
 */
UCLASS(NotBlueprintable, NotBlueprintType)
class PROCEDURALMAPS_API ULimenMapAlgorithm : public UObject, public FRunnable
{
	GENERATED_BODY()

public:
	DECLARE_DELEGATE_ThreeParams(FAlgorithmFinish, bool /* bSuccess */,
								 const FGuid& /* MapId */,
								 ULimenProceduralMap* /* Map */);
	
	void CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters, FAlgorithmFinish& FinishCallback);
	ULimenProceduralMap* CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters);
	template<typename T>
	T* CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters)
	{
		ULimenProceduralMap* OutMap = CreateMap(MapId, MapParameters);
		return CastChecked<T>(OutMap);
	}

	virtual TSubclassOf<ULimenProceduralMap> GetGeneratedMapClass() const;
	
protected:
	/**
	 * @brief Override to generate the map with the given parameters.
	 * Runs on a new thread.
	 * @param MapParameters The parameters for the map generation.
	 * @param OutGeneratedMap A pointer to the generated map.
	 */
	virtual void GenerateMap(const UProceduralMapParameters* MapParameters, ULimenProceduralMap*& OutGeneratedMap);

	virtual bool Init() override;
	virtual uint32 Run() override;
	virtual void Stop() override;
	virtual void Exit() override;

private:
	TStrongObjectPtr<ULimenProceduralMap> GeneratedMap;
	TWeakObjectPtr<const UProceduralMapParameters> GeneratedMapParameters;

	FGuid GeneratedMapId;
	FAlgorithmFinish* OnAlgorithmFinished;
	
	TUniquePtr<FRunnableThread> MapGenerationThread;
};
