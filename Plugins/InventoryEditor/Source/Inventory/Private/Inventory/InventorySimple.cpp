#include "InventorySimple.h"

UItemStack* UInventorySimple::GetItemStackFromSlot(int32 SlotIndex) {
  if (!CheckSlot(SlotIndex)) return nullptr;
  return Slots[SlotIndex]->ItemStack;
}

UItemStack* UInventorySimple::GetItemStackFromSlotArray(int32 ArrayIndex, int32 X, int32 Y) {
  if (!CheckArray(ArrayIndex, X, Y)) return nullptr;
  return SlotArrays[ArrayIndex]->GetItemStackAt(X, Y);
}

bool UInventorySimple::PlaceItemStackIntoSlot(UItemStack* ItemStack, int32 Index) {
  if (!CheckSlot(Index)) return false;
  if (Slots[Index]->ItemStack) return false;
  // TODO: Change Outer
  Slots[Index]->ItemStack = ItemStack;
  return true;
}

bool UInventorySimple::PlaceItemStackIntoArray(UItemStack* ItemStack, int32 ArrayIndex, int32 X, int32 Y) {
  if (!CheckArray(ArrayIndex)) return false;
  UInventorySlotArray* array = SlotArrays[ArrayIndex];
  // TODO: Change Outer
  for (int32 i = 0; i < ItemStack->Item->Height; i++) {
    for (int32 j = 0; j < ItemStack->Item->Width; j++) {
      if (!CheckArray(ArrayIndex, X + i, Y + j)) {
        return false;
      }
      if (array->GetItemStackAt(X + i, Y + j)) {
        return false;
      }
    }
  }
  for (int32 i = 0; i < ItemStack->Item->Height; i++) {
    for (int32 j = 0; j < ItemStack->Item->Width; j++) {
      array->SetItemStackAt(ItemStack, X + i, Y + j);
    }
  }
  return true;
}

bool UInventorySimple::MergeItemStackIntoSlot(UItemStack* ItemStack, int32 Index) {
  if (!CheckSlot(Index)) return false;

  UItemStack* otherStack = Slots[Index]->ItemStack;
  if (otherStack && otherStack->CanMerge(ItemStack)) {
    otherStack->MergeWith(ItemStack);
    return true;
  }
  return false;
}

bool UInventorySimple::MergeItemStackIntoArray(UItemStack* ItemStack, int32 ArrayIndex, int32 X, int32 Y) {
  if (!CheckArray(ArrayIndex, X, Y)) return false;

  UItemStack* otherStack = SlotArrays[ArrayIndex]->GetItemStackAt(X, Y);
  if (otherStack && otherStack->CanMerge(ItemStack)) {
    otherStack->MergeWith(ItemStack);
    return true;
  }
  return false;
}

UItemStack* UInventorySimple::RemoveItemStackFromSlot(int32 Index) {
  if (!CheckSlot(Index)) return nullptr;
  UItemStack* stackToRemove = Slots[Index]->ItemStack;
  Slots[Index]->ItemStack = nullptr;
  return stackToRemove;
}

UItemStack* UInventorySimple::RemoveItemStackFromArray(int32 ArrayIndex, int32 X, int32 Y) {
  if (!CheckArray(ArrayIndex, X, Y)) return nullptr;

  UInventorySlotArray* array = SlotArrays[ArrayIndex];
  UItemStack* stackToRemove = array->GetItemStackAt(X, Y);
  if (!stackToRemove) return nullptr;

  // remove all occurrence of this stack
  for (int32 i = 0; i < array->Height; i++) {
    for (int32 j = 0; j < array->Width; j++) {
      if (array->GetItemStackAt(i, j) == stackToRemove) {
        array->SetItemStackAt(nullptr, i, j);
      }
    }
  }

  return stackToRemove;
}

void UInventorySimple::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) {
  SynchronizeStyle();
}

void UInventorySimple::GetItemStackPositionsAndSizes(TArray<UItemStack*>& OutStacks, TArray<FVector2D>& OutPositions, TArray<FVector2D>& OutSize) {
  for (int32 i = 0; i < Slots.Num(); i++) {
    if (Slots[i] && Slots[i]->ItemStack) {
      OutStacks.Add(Slots[i]->ItemStack);
      FVector2D slotPos = Style.SlotStartPosition[i];
      FVector2D slotSize = Style.SlotSize[i];
      FVector2D itemSize = FVector2D(Slots[i]->ItemStack->Item->Width,
                                     Slots[i]->ItemStack->Item->Height) * Style.ArraySlotWidth;
      FVector2D itemPos = (slotSize - itemSize) / 2 + slotPos;

      OutPositions.Add(itemPos);
      OutSize.Add(itemSize);
    }
  }
  for (int32 i = 0; i < SlotArrays.Num(); i++) {
    if (!SlotArrays[i]) continue;
    TMap<UItemStack*, FVector2D> stackPos;
    for (int r = 0; r < SlotArrays[i]->Height; r++) {
      for (int c = 0; c < SlotArrays[i]->Width; c++) {
        UItemStack* stack = SlotArrays[i]->GetItemStackAt(r, c);
        if (stack && !stackPos.Contains(stack)) {
          stackPos.Add(stack, FVector2D(c, r));
        }
      }
    }
    for (auto& elem : stackPos) {
      OutStacks.Add(elem.Key);
      OutPositions.Add(Style.SlotArrayStartPosition[i] + elem.Value * Style.ArraySlotWidth);
      OutSize.Add(FVector2D(elem.Key->Item->Width, elem.Key->Item->Height)*Style.ArraySlotWidth);
    }
  }
}

void UInventorySimple::GetItemStackPositionsInSlotArrays(UItemStack* Stack, TArray<FVector2D>& OutPositions) {
  for (int32 i = 0; i < SlotArrays.Num(); i++) {
    if (!SlotArrays[i]) continue;

    for (int32 r = 0; r < SlotArrays[i]->Height; r++) {
      for (int32 c = 0; c < SlotArrays[i]->Width; c++) {
        if (Stack == SlotArrays[i]->GetItemStackAt(r, c)) {
          OutPositions.Add(Style.SlotArrayStartPosition[i] + FVector2D(c, r) * Style.ArraySlotWidth);
        }
      }
    }
  }
}

void UInventorySimple::SynchronizeStyle() {
  if (Style.SlotStartPosition.Num() <= Slots.Num()) {
    Style.SlotStartPosition.SetNum(Slots.Num());
    Style.SlotSize.SetNum(Slots.Num());
  } else {
    for (int32 i = Style.SlotStartPosition.Num(); i < Slots.Num(); i++) {
      Style.SlotStartPosition.Add(FVector2D(0, 0));
      Style.SlotSize.Add(FVector2D(0, 0));
    }
  }
  if (Style.SlotArrayStartPosition.Num() <= SlotArrays.Num()) {
    Style.SlotArrayStartPosition.SetNum(SlotArrays.Num());
  } else {
    for (int32 i = Style.SlotArrayStartPosition.Num(); i < SlotArrays.Num(); i++) {
      Style.SlotArrayStartPosition.Add(FVector2D(0, 0));
    }
  }
}
