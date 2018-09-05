#include "InventorySimpleFactory.h"
#include "InventorySimple.h"
#include "Object.h"

UInventorySimpleFactory::UInventorySimpleFactory(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer) {

  bCreateNew = true;
  bEditAfterNew = true;
  SupportedClass = UInventorySimple::StaticClass();
}

UObject* UInventorySimpleFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) {
  UInventorySimple* NewInventorySimple = NewObject<UInventorySimple>(InParent, Class, Name, Flags | RF_Transactional);
  return NewInventorySimple;
}
