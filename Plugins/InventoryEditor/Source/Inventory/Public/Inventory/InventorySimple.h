#pragma once

#include "CoreMinimal.h"
#include "Inventory/InventoryBase.h"
#include "Inventory/InventorySlotArray.h"
#include "Inventory/InventorySlotShapeless.h"
#include "UI/InventoryStyle.h"
#include "InventorySimple.generated.h"

UCLASS(Blueprintable)
class UInventorySimple : public UInventoryBase {
  GENERATED_BODY()
  public:

  // UItemStack* GetItemStackAt(int32 x, int32 y);
  UFUNCTION(BlueprintCallable, Category = Inventory)
  UItemStack* GetItemStackFromSlot(int32 SlotIndex);
  UFUNCTION(BlueprintCallable, Category = Inventory)
  UItemStack* GetItemStackFromSlotArray(int32 ArrayIndex, int32 X, int32 Y);

  UPROPERTY(EditAnywhere, Instanced, Category = Inventory)
  TArray<UInventorySlotBase*> Slots;

  UPROPERTY(EditAnywhere, Instanced, Category = Inventory)
  TArray<UInventorySlotArray*> SlotArrays;

  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual bool PlaceItemStackIntoSlot(UItemStack* ItemStack, int32 Index);
  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual bool PlaceItemStackIntoArray(UItemStack* ItemStack, int32 ArrayIndex, int32 X, int32 Y);

  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual bool MergeItemStackIntoSlot(UItemStack* ItemStack, int32 Index);
  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual bool MergeItemStackIntoArray(UItemStack* ItemStack, int32 ArrayIndex, int32 X, int32 Y);

  // UItemStack* RemoveItemStackAt(int x, int y);
  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual UItemStack* RemoveItemStackFromSlot(int32 Index);
  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual UItemStack* RemoveItemStackFromArray(int32 ArrayIndex, int32 X, int32 Y);
  
  UPROPERTY(EditAnywhere, Category = Appearance)
  FInventoryStyleSimple Style;

  virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;

  void GetItemStackPositionsAndSizes(TArray<UItemStack*>& OutStacks, TArray<FVector2D>& OutPositions, TArray<FVector2D>& OutSize);

protected:
  void SynchronizeStyle() {
    Style.SlotStartPosition.SetNum(Slots.Num());
    Style.SlotSize.SetNum(Slots.Num());
    Style.SlotArrayStartPosition.SetNum(SlotArrays.Num());
  }

  bool CheckSlot(int32 Index) {
    return Index >= 0 && Index < Slots.Num();
  }
  bool CheckArray(int32 ArrayIndex, int32 X=0, int32 Y=0) {
    if (ArrayIndex < 0 || ArrayIndex >= SlotArrays.Num()) return false;
    UInventorySlotArray* array = SlotArrays[ArrayIndex];
    if (!array) return false;
    return X >= 0 && Y >= 0 && X < array->Height && Y < array->Width;
  }
};

