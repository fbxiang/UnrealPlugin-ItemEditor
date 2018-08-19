#include "Item/ItemDatabase.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "ItemEditor"

UItemDatabase::UItemDatabase(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer) {}

UItemCategory::UItemCategory(const FObjectInitializer& ObjectInitializer)
  : Super(ObjectInitializer) {}

UItemCategory* UItemDatabase::AddCategory() {
  UItemCategory* newCategory = NewObject<UItemCategory>(this, FName(""), RF_Transactional);

  GEditor->BeginTransaction(LOCTEXT("AddCategory", "Add category"));
  Modify();
  ItemCategoryList.Add(newCategory);
  GEditor->EndTransaction();

  return newCategory;
}

void UItemDatabase::DeleteCategory(UItemCategory* Category) {
  GEditor->BeginTransaction(LOCTEXT("DeleteCategory", "Delete category"));
  Modify();
  ItemCategoryList.Remove(Category);
  GEditor->EndTransaction();
}

UItem* UItemDatabase::AddItemToCategory(FName CategoryName) {
  return AddItemToCategory(FindCategory(CategoryName));
}

UItem* UItemDatabase::AddItemToCategory(UItemCategory* Category) {
  if (!Category) return nullptr;

  UItem* newItem = NewObject<UItem>(this, FName(""), RF_Transactional);

  GEditor->BeginTransaction(LOCTEXT("AddItemToCategory", "Add item to category"));
  Category->Modify();
  Category->AddItem(newItem);
  GEditor->EndTransaction();

  return newItem;
}

void UItemDatabase::RemoveItemFromCategory(UItemCategory* Category, UItem* Item) {
  if (!Category || !Item) return;
  
  GEditor->BeginTransaction(LOCTEXT("AddItemToCategory", "Add item to category"));
  Category->Modify();
  Category->DeleteItem(Item);
  GEditor->EndTransaction();
}

UItemCategory* UItemDatabase::FindCategory(FName CategoryName) {
  for (int32 i = 0; i < ItemCategoryList.Num(); i++) {
    if (ItemCategoryList[i]->GetCategoryName() == CategoryName) {
      return ItemCategoryList[i];
    }
  }
  return nullptr;
}

#undef LOCTEXT_NAMESPACE
