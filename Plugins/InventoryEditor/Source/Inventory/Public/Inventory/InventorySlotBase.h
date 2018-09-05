#pragma once

#include "CoreMinimal.h"
#include "ItemStack.h"
#include "InventorySlotBase.generated.h"

UCLASS(Blueprintable, Abstract)
class UInventorySlotBase : public UObject {
  GENERATED_BODY()
  public:

  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual bool CanFit(UItemStack* InItemStack) {
    return false;
  }

  UPROPERTY(BlueprintReadOnly, Category = Inventory)
  UItemStack* ItemStack;
};
