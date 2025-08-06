// Copyright Epic Games, Inc. All Rights Reserved.

#include "LimenCore.h"

#include "ShaderCore.h"
#include "Misc/Paths.h"


#define LOCTEXT_NAMESPACE "FLimenCoreModule"

void FLimenCoreModule::StartupModule()
{
	const FString PluginShaderDir = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("LimenFramework"), TEXT("LimenCore"), TEXT("Shaders"));
	AddShaderSourceDirectoryMapping(TEXT("/LimenCore"), PluginShaderDir);
}

void FLimenCoreModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLimenCoreModule, LimenCore)