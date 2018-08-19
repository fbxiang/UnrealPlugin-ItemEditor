#include "ItemEditor/ItemEditorCommands.h"

#define LOCTEXT_NAMESPACE "ItemEditor"

void FItemEditorCommands::RegisterCommands()
{
  UI_COMMAND(CreateCategory, "Create a new category", "Create a new category", EUserInterfaceActionType::Button, FInputChord());

  UI_COMMAND(RenameCategory, "Rename a category", "Rename a category", EUserInterfaceActionType::Button, FInputChord());

  UI_COMMAND(DeleteCategory, "Delete a category", "Create a category", EUserInterfaceActionType::Button, FInputChord());

  UI_COMMAND(CreateItem, "Create a new item", "Create a new item", EUserInterfaceActionType::Button, FInputChord());

  UI_COMMAND(DeleteItem, "Delete an item", "Delete an item", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
