// Copyright © FaceSoftware. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"
#include "RenderGraphResources.h"
#include "SceneView.h"


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
		SHADER_PARAMETER(uint32,    Mask)

		// Output flag (1 uint)
		SHADER_PARAMETER_RDG_BUFFER_UAV(RWBuffer<uint>, OutFlag)
		SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float4>, DebugOut)
	END_SHADER_PARAMETER_STRUCT()

	static bool ShouldCompilePermutation(const FGlobalShaderPermutationParameters& Params)
	{
		return IsFeatureLevelSupported(Params.Platform, ERHIFeatureLevel::SM5);
	}
};
