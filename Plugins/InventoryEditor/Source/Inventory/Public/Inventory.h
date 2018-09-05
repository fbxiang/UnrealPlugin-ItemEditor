#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"

class FInventoryModule : public IModuleInterface
{
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;
public:
  void RegisterSettings();
  void UnregisterSettings();
};
