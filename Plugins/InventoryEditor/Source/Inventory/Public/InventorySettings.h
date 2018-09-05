#pragma once

#include "CoreMinimal.h"
#include "ItemDatabase.h"
#include "InventorySettings.generated.h"

UCLASS(Config = Game)
class UInventorySettings : public UObject {
  GENERATED_BODY()

public:
  static const FName ContainerName;
  static const FName CategoryName;
  static const FName SectionName;

public:
  UPROPERTY(config, EditAnywhere, Category = Registry, meta = (AllowedClasses = "UItemDatabase"))
  FSoftObjectPath ItemRegistry;

  static UItemDatabase* GetItemRegistry(); 
};
