#pragma once

#include "CoreMinimal.h"
#include "InventorySlotElement.h"
#include "InventorySlotArray.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class UInventorySlotArray : public UObject {
  GENERATED_BODY()
  public:

  UFUNCTION(BlueprintCallable, Category = Inventory)
  int32 Num() { return Width * Height; };

  UPROPERTY()
  TArray<UInventorySlotElement*> Slots;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 Width;

  UPROPERTY(EditAnywhere, BlueprintReadOnly)
  int32 Height;

  UInventorySlotElement* GetSlotAt(int x, int y) {
    RegenerateSlotsIfNeeded();
    if (x >= 0 && x < Height && y >= 0 && y < Width) {
      return Slots[x * Width + y];
    }
    return nullptr;
  }

  UFUNCTION(BlueprintCallable, Category = Inventory)
  UItemStack* GetItemStackAt(int x, int y) {
    RegenerateSlotsIfNeeded();
    if (x >= 0 && x < Height && y >= 0 && y < Width) {
      return Slots[x * Width + y]->ItemStack;
    }
    return nullptr;
  }

  void SetItemStackAt(UItemStack* ItemStack, int x, int y) {
    RegenerateSlotsIfNeeded();
    if (x >= 0 && x < Height && y >= 0 && y < Width) {
      Slots[x * Width + y]->ItemStack = ItemStack;
    }
  }

private:
  void RegenerateSlotsIfNeeded() {
    if (Slots.Num() != Width * Height) {
      Slots.SetNum(0);
      Slots.SetNum(Width * Height);
      for (int32 i = 0; i < Width * Height; i++) {
        Slots[i] = NewObject<UInventorySlotElement>(this);
        Slots[i]->ItemStack = nullptr;
      }
    }
  }
};
