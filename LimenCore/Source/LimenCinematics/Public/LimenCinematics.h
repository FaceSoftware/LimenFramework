#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FLimenCinematicsModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;
};
