#include "InventorySimpleEditor/InventorySimpleEditorCommands.h"

#define LOCTEXT_NAMESPACE "InventorySimpleEditor"

void FInventorySimpleEditorCommands::RegisterCommands() {
  UI_COMMAND(SetShowSlots, "Slots", "Displays the Position of Slots", EUserInterfaceActionType::ToggleButton, FInputChord());
}


#undef LOCTEXT_NAMESPACE
