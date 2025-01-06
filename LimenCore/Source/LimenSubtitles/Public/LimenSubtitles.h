#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLimenSubtitlesModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
