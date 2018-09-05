#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "InventoryEditorStyle.h"

class FInventorySimpleEditorCommands : public TCommands<FInventorySimpleEditorCommands> {
  public:

  FInventorySimpleEditorCommands()
  : TCommands<FInventorySimpleEditorCommands>
  (TEXT("InventorySimpleEditor"), NSLOCTEXT("Contexts", "InventorySimpleEditor", "Simple Inventory Editor"),
   NAME_None, FInventoryEditorStyle::Get()->GetStyleSetName()) { }

  virtual void RegisterCommands() override;
  
public:
  TSharedPtr<FUICommandInfo> SetShowSlots;
};

