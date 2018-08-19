
#include "ItemDatabaseFactory.h"
#include "Item/ItemDatabase.h"
#include "Object.h"

#define LOCTEXT_NAMESPACE "ItemDatabase"

UItemDatabaseFactory::UItemDatabaseFactory(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer)
{
  bCreateNew = true;
  bEditAfterNew = true;
  SupportedClass = UItemDatabase::StaticClass();
}

UObject* UItemDatabaseFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) {
  UItemDatabase* NewItemDatabase = NewObject<UItemDatabase>(InParent, Class, Name, Flags | RF_Transactional);
  return NewItemDatabase;
}

#undef LOCTEXT_NAMESPACE
