#pragma once

#include "CoreMinimal.h"
#include "InventorySlotBase.h"
#include "InventorySlotElement.generated.h"

UCLASS()
class UInventorySlotElement : public UInventorySlotBase {
  GENERATED_BODY()
  public:

  virtual bool CanFit(UItemStack* InItemStack) override {
    return true;
  }
};
