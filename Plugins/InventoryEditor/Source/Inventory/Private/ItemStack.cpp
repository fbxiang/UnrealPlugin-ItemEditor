#include "ItemStack.h"
#include "InventorySettings.h"
#include "ISettingsModule.h"
#include "ISettingsContainer.h"
#include "ISettingsCategory.h"
#include "ISettingsSection.h"

bool UItemStack::CanMerge(UItemStack* Other) {
  return Item == Other->Item;
}

void UItemStack::MergeWith(UItemStack* Other) {
  if (!CanMerge(Other)) return;
  int32 total = this->StackSize + Other->StackSize;
  if (total <= Item->MaxStackSize) {
    this->StackSize = total;
    Other->StackSize = 0;
  } else {
    this->StackSize = Item->MaxStackSize;
    Other->StackSize = total - Item->MaxStackSize;
  }
}
