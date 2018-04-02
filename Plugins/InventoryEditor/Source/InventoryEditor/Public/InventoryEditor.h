#pragma once

#include "CoreMinimal.h"
#include "ModuleManager.h"
#include "ItemEditor/ItemEditor.h"
#include "ItemEditor/ItemDatabaseAssetActions.h"

class FInventoryEditorModule : public IModuleInterface
{
public:
  virtual void StartupModule() override;
  virtual void ShutdownModule() override;
private:
  TArray< TSharedPtr<IAssetTypeActions> > CreatedAssetTypeActions;

  void RegisterAssetTypeAction(IAssetTools& AssetTools, TSharedRef<IAssetTypeActions> Action);
};

