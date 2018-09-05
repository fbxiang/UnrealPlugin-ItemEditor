#pragma once

#include "CoreMinimal.h"
#include "InventoryBase.h"
#include "InventorySlotShapeless.generated.h"

UCLASS(Blueprintable, EditInlineNew)
class UInventorySlotShapeless : public UInventorySlotBase {
  GENERATED_BODY()
  public:

  virtual bool CanFit(UItemStack* InItemStack) override {
    return true;
  }
};
