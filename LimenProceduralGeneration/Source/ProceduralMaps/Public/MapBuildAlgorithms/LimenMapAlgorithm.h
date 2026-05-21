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

	ULimenMapAlgorithm();
	virtual void BeginDestroy() override;
	
	void CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters, FAlgorithmFinish& FinishCallback);
	ULimenProceduralMap* CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters);
	template<typename T>
	T* CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters)
	{
		ULimenProceduralMap* OutMap = CreateMap(MapId, MapParameters);
		return CastChecked<T>(OutMap);
	}
	
	virtual TSubclassOf<ULimenProceduralMap> GetGeneratedMapClass() const PURE_VIRTUAL(ULimenMapAlgorithm::GetGeneratedMapClass, return nullptr;)
	
protected:
	/**
	 * @brief Override to generate the map with the given parameters.
	 * Runs on a new thread.
	 * @param MapParameters The parameters for the map generation.
	 * @param OutGeneratedMap A pointer to the generated map.
	 */
	virtual void GenerateMap(const UProceduralMapParameters* MapParameters, ULimenProceduralMap* OutGeneratedMap) PURE_VIRTUAL(ULimenMapAlgorithm::GenerateMap);

private:
	TStrongObjectPtr<ULimenProceduralMap> GeneratedMap;
	TWeakObjectPtr<const UProceduralMapParameters> GeneratedMapParameters;

	FGuid GeneratedMapId;
	FAlgorithmFinish* OnAlgorithmFinished;
	
	TUniquePtr<FRunnableThread> MapGenerationThread;
	
	bool Init() final override;
	uint32 Run() final override;
	void Stop() final override;
	void Exit() final override;
};
