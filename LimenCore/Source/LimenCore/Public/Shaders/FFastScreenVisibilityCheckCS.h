// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"


DECLARE_UNIFORM_BUFFER_STRUCT(FSceneTextureUniformParameters, ENGINE_API)

class LIMENCORE_API FFastScreenVisibilityCheckCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FFastScreenVisibilityCheckCS);
	SHADER_USE_PARAMETER_STRUCT(FFastScreenVisibilityCheckCS, FGlobalShader);

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
		// View UB (non-RDG is fine)
		SHADER_PARAMETER_STRUCT_REF(FViewUniformShaderParameters, View)
		SHADER_PARAMETER_RDG_UNIFORM_BUFFER(FSceneTextureUniformParameters, SceneTextures)

		// Your custom params
		SHADER_PARAMETER(FIntPoint, ViewRectMin)
		SHADER_PARAMETER(FIntPoint, ViewRectSize)
		SHADER_PARAMETER_RDG_BUFFER_SRV(StructuredBuffer<uint32>, Masks)

		// Output flag (uint)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWStructuredBuffer<uint32>, OutFlag)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Params)
	{
		return IsFeatureLevelSupported(Params.Platform, ERHIFeatureLevel::SM5);
	}
};
