#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventorySimple.h"
#include "InventoryItemPicker.generated.h"

UCLASS()
class UInventoryItemPicker : public UInventorySimple {
  GENERATED_UCLASS_BODY()
  public:
  void SetSearchKey(const FText& Key); 

  void StartSearch();

  int32 NumPages();

  int32 GetPage();

  void SetPage(int32 InPage);

  // place is not allowed
  virtual bool PlaceItemStackIntoSlot(UItemStack* ItemStack, int32 Index) override {
    return true;
  }
  virtual bool PlaceItemStackIntoArray(UItemStack* ItemStack, int32 ArrayIndex, int32 X, int32 Y) override {
    return true;
  }

  // merge is not allowed
  virtual bool MergeItemStackIntoSlot(UItemStack* ItemStack, int32 Index) {
    return true;
  }
  virtual bool MergeItemStackIntoArray(UItemStack* ItemStack, int32 ArrayIndex, int32 X, int32 Y) {
    return true;
  }

  virtual UItemStack* RemoveItemStackFromSlot(int32 Index) override;
  virtual UItemStack* RemoveItemStackFromArray(int32 ArrayIndex, int32 X, int32 Y) override;

private:
  void GeneratePages();
  void FillPage();
  void GenerateItemStackList(); 
  void GenerateItemStackListIncremental(); 

  int32 Page;
  FText SearchKey;
  TArray<UItemStack*> ItemStacks;
  TArray<int32> PageIndices;
};
