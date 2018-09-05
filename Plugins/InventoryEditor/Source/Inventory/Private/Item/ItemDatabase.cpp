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

void UItemDatabase::MoveUpCategory(UItemCategory* Category) {
  int32 index = ItemCategoryList.Find(Category);
  if (index == 0) return;
  GEditor->BeginTransaction(LOCTEXT("MoveUpCategory", "Move Category"));
  Modify();
  UItemCategory* other = ItemCategoryList[index-1];
  ItemCategoryList[index-1] = ItemCategoryList[index];
  ItemCategoryList[index] = other;
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
  void RemoveItem(UItem* Item);
  Category->AddItem(newItem);
  GEditor->EndTransaction();

  return newItem;
}

void UItemDatabase::RemoveItemFromCategory(UItemCategory* Category, UItem* Item) {
  if (!Category || !Item) return;
  
  GEditor->BeginTransaction(LOCTEXT("RemoveItemFromCategory", "Remove item from category"));
  Category->Modify();
  Category->DeleteItem(Item);
  GEditor->EndTransaction();
}

void UItemDatabase::RemoveItem(UItem* Item) {
  if (!Item) return;
  UItemCategory* category;
  for (int32 i = 0; i < ItemCategoryList.Num(); i++) {
    if (ItemCategoryList[i]->Contains(Item)) {
      category = ItemCategoryList[i];
    }
  }
  if (category) {
    RemoveItemFromCategory(category, Item);
  }
}

void UItemDatabase::MoveUpItem(UItem* Item) {
  if (!Item) return;
  UItemCategory* category;
  for (int32 i = 0; i < ItemCategoryList.Num(); i++) {
    if (ItemCategoryList[i]->Contains(Item)) {
      category = ItemCategoryList[i];
    }
  }
  if (category) {
    GEditor->BeginTransaction(LOCTEXT("MoveUpItem", "Move item up"));
    category->Modify();
    category->MoveUpItem(Item);
    GEditor->EndTransaction();
  }
}


UItemCategory* UItemDatabase::FindCategory(FName CategoryName) {
  for (int32 i = 0; i < ItemCategoryList.Num(); i++) {
    if (ItemCategoryList[i]->GetCategoryName() == CategoryName) {
      return ItemCategoryList[i];
    }
  }
  return nullptr;
}

UItem* UItemDatabase::FindItemByName(FName Name) {
  for (int32 i = 0; i < ItemCategoryList.Num(); i++) {
    if (UItem* item = ItemCategoryList[i]->FindItemByName(Name)) {
      return item;
    }
  }
  return nullptr;
}

void UItemCategory::SearchItems(FText Key, TArray<UItem*>& Out) {
  for (int32 i = 0; i < Items.Num(); i++) {
    FString searchString = Key.ToString();
    FString uniqueString = Items[i]->UniqueName.ToString();
    FString displayString = Items[i]->DisplayName.ToString();

    if (searchString.Len() == 0 ||
        uniqueString.Contains(searchString,
                              ESearchCase::Type::IgnoreCase,
                              ESearchDir::Type::FromStart) ||
        displayString.Contains(searchString,
                               ESearchCase::Type::IgnoreCase,
                               ESearchDir::Type::FromStart)) {
      Out.Add(Items[i]);
    }
  }
}

void UItemDatabase::SearchItems(FText Key, TArray<UItem*>& Out) {
  for (int32 i = 0; i < ItemCategoryList.Num(); ++i) {
    ItemCategoryList[i]->SearchItems(Key, Out);
  }
}

#undef LOCTEXT_NAMESPACE
