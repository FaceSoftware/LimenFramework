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
	FShaderPipelineCache::PauseBatching();
	FShaderPipelineCache::BatchMode	BatchMode(FShaderPipelineCache::BatchMode::Fast);

	switch (InContext)
	{
	case EContext::LoadingScreen:
		{
			BatchMode = FShaderPipelineCache::BatchMode::Precompile;
		}
		break;

	case EContext::UI:
		{
			BatchMode = FShaderPipelineCache::BatchMode::Fast;
		}
		break;

	case EContext::InGame:
		{
			BatchMode = FShaderPipelineCache::BatchMode::Background;
		}
		break;
	}

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
