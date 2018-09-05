#pragma once

#include "CoreMinimal.h"
#include "InventoryComponentBase.h"
#include "InventorySimple.h"
#include "InventoryComponentSimple.generated.h"

UCLASS(Blueprintable, ClassGroup=(Inventory), meta=(BlueprintSpawnableComponent))
class UInventoryComponentSimple : public UInventoryComponentBase {
  GENERATED_BODY()
  public:

  virtual UInventoryBase* GetInventory() override { return Inventory; }

  UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Inventory)
  UInventorySimple* Inventory;
};
