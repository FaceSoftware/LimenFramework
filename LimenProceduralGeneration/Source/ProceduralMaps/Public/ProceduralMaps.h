#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

DECLARE_LOG_CATEGORY_EXTERN(LogLimenProceduralGeneration, Log, All);
inline DEFINE_LOG_CATEGORY(LogLimenProceduralGeneration);

class FProceduralMapsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
