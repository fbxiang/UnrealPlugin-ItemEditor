#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"

class FItemEditorCommands : public TCommands<FItemEditorCommands>
{
public:
  FItemEditorCommands()
    : TCommands<FItemEditorCommands>
    (TEXT("ItemEditor"),
     NSLOCTEXT("Contexts", "InventoryEditor", "Item Editor"),
     NAME_None,
     "")
    {}
  
  virtual void RegisterCommands() override;

public:
  TSharedPtr<FUICommandInfo> CreateCategory;
  TSharedPtr<FUICommandInfo> RenameCategory;
  TSharedPtr<FUICommandInfo> DeleteCategory;

  TSharedPtr<FUICommandInfo> CreateItem;
  TSharedPtr<FUICommandInfo> DeleteItem;
};
