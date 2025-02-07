// Copyright © 2024 FaceSoftware. All rights reserved.


#include "PSO/LimenPSOCompiler.h"

#include "ShaderPipelineCache.h"


FLimenPSOCompiler::FLimenPSOCompiler()
{
}

FLimenPSOCompiler* FLimenPSOCompiler::Get()
{
	static FLimenPSOCompiler Instance;
	return &Instance;
}

FLimenPSOCompiler::~FLimenPSOCompiler()
{
}

void FLimenPSOCompiler::StartBatching(const EContext InContext)
{
	FShaderPipelineCache::BatchMode	BatchMode(FShaderPipelineCache::BatchMode::Fast);

	switch (InContext)
	{
	case EContext::LoadingScreen:
		{
			BatchMode = FShaderPipelineCache::BatchMode::Fast;
		}
		break;

	case EContext::UI:
		{
			BatchMode = FShaderPipelineCache::BatchMode::Background;
		}
		break;

	case EContext::InGame:
		{
			FShaderPipelineCache::PauseBatching();
		}
		return;
	}

	
	FShaderPipelineCache::PauseBatching();
	FShaderPipelineCache::SetBatchMode(BatchMode);
	FShaderPipelineCache::ResumeBatching();
}

void FLimenPSOCompiler::StopBatching()
{
	FShaderPipelineCache::PauseBatching();
}

bool FLimenPSOCompiler::IsBatchingRunning()
{
	return !FShaderPipelineCache::IsBatchingPaused();
}
