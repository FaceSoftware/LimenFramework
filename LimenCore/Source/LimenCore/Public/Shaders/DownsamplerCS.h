// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterStruct.h"

/**
 * 
 */
class LIMENCORE_API FDownsamplerCS : public FGlobalShader
{
public:
	DECLARE_GLOBAL_SHADER(FDownsamplerCS);
	SHADER_USE_PARAMETER_STRUCT(FDownsamplerCS, FGlobalShader)

	BEGIN_SHADER_PARAMETER_STRUCT(FParameters, )
			SHADER_PARAMETER_RDG_TEXTURE(Texture2D<float4>, Input)
			SHADER_PARAMETER_SAMPLER(SamplerState, InputSampler)
			SHADER_PARAMETER_RDG_TEXTURE_UAV(RWTexture2D<float>, Result)
	END_SHADER_PARAMETER_STRUCT()
};
