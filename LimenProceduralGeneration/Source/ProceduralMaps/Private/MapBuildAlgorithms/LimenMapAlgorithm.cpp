// Copyright Face Software. All Rights Reserved.


#include "MapBuildAlgorithms/LimenMapAlgorithm.h"

#include "Async/Async.h"
#include "DataAssets/ProceduralMapParameters.h"
#include "HAL/RunnableThread.h"
#include "LogMacros/LimenLogMacros.h"
#include "Maps/LimenProceduralMap.h"


ULimenMapAlgorithm::ULimenMapAlgorithm() : Super()
{
}

void ULimenMapAlgorithm::BeginDestroy()
{
	Super::BeginDestroy();

	GeneratedMap.Reset();
}

void ULimenMapAlgorithm::CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters, FAlgorithmFinish& FinishCallback)
{
	if (MapGenerationThread.IsValid())
	{
		MapGenerationThread->WaitForCompletion();
	}

	GeneratedMapParameters = MapParameters;
	GeneratedMapId = MapId;
	OnAlgorithmFinished = &FinishCallback;
	
	if (GeneratedMap != nullptr)
	{
		GeneratedMap->ConditionalBeginDestroy();
	}
	
	GeneratedMap = TStrongObjectPtr(NewObject<ULimenProceduralMap>(this, GetGeneratedMapClass()));

	FRunnableThread* NewThread = FRunnableThread::Create(this, TEXT("MapGenerationThread"));
	MapGenerationThread.Reset(NewThread);
}

ULimenProceduralMap* ULimenMapAlgorithm::CreateMap(const FGuid& MapId, const UProceduralMapParameters* MapParameters)
{
	GeneratedMapParameters = MapParameters;
	GeneratedMapId = MapId;
	
	if (GeneratedMap != nullptr)
	{
		GeneratedMap->ConditionalBeginDestroy();
	}
	
	GeneratedMap = TStrongObjectPtr(NewObject<ULimenProceduralMap>(this, GetGeneratedMapClass()));
	ULimenProceduralMap* Map = GeneratedMap.Get();
	GenerateMap(GeneratedMapParameters.Get(), Map);

	return GeneratedMap.Get();
}

TSubclassOf<ULimenProceduralMap> ULimenMapAlgorithm::GetGeneratedMapClass() const
{ 
	PURE_VIRTUAL(ULimenMapAlgorithm::GetGeneratedMapClass);
	return nullptr;
}

void ULimenMapAlgorithm::GenerateMap(const UProceduralMapParameters* MapParameters, ULimenProceduralMap*& OutGeneratedMap)
{
	PURE_VIRTUAL(ULimenMapAlgorithm::GenerateMap);
}

bool ULimenMapAlgorithm::Init()
{
#if WITH_EDITOR

	checkf(GeneratedMapId.IsValid(), TEXT("Map id is invalid!"));
	checkf(GeneratedMapParameters->ValidateParameters(), TEXT("Map parameters are invalid"));
	return true;

#else

	return GeneratedMapId.IsValid() && GeneratedMapParameters->ValidateParameters();

#endif
}

uint32 ULimenMapAlgorithm::Run()
{
	AsyncTask(ENamedThreads::Type::GameThread, [this]
	{
		LIMEN_LOG(LogLimen, Log, this, TEXT("Started algorithm..."));
	});

	ULimenProceduralMap* Map = GeneratedMap.Get();
	GenerateMap(GeneratedMapParameters.Get(), Map);
	return 0;
}

void ULimenMapAlgorithm::Stop()
{
	
}

void ULimenMapAlgorithm::Exit()
{
	AsyncTask(ENamedThreads::GameThread, [this]
	{
		if (!OnAlgorithmFinished) return;

		OnAlgorithmFinished->Execute(true, GeneratedMapId, GeneratedMap.Get());
		LIMEN_LOG(LogLimen, Log, this, TEXT("Finished algorithm"));
	});
}
