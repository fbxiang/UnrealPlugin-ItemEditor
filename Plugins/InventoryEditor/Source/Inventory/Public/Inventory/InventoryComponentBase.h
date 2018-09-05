#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponentBase.generated.h"

UCLASS(Abstract)
class UInventoryComponentBase : public UActorComponent {
  GENERATED_BODY()
  public:

  UFUNCTION(BlueprintCallable, Category = Inventory)
  virtual UInventoryBase* GetInventory() { return nullptr; };
};
