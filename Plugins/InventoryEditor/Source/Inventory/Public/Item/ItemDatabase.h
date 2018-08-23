#pragma once

#include "CoreMinimal.h"
#include "Item/Item.h"
#include "Misc/ITransaction.h"
#include "ItemDatabase.generated.h"

extern CORE_API ITransaction* GUndo;

UCLASS()
class UItemCategory : public UObject {
  GENERATED_UCLASS_BODY()
public:
  UFUNCTION()
  FName GetCategoryName() { return CategoryName; }

  UFUNCTION()
  void SetCategoryName(const FName& NewName) { CategoryName = NewName; }

  UFUNCTION()
  void AddItem(UItem* Item) { Items.Add(Item); }

  UFUNCTION()
  void DeleteItem(UItem* Item) { Items.Remove(Item); }

  UFUNCTION()
  bool Contains(UItem* Item) { return Items.Contains(Item); }

  void MoveUpItem(UItem* Item) {
    int32 index = Items.Find(Item);
    if (index != INDEX_NONE && index != 0) {
      UItem* other = Items[index - 1];
      Items[index - 1] = Items[index];
      Items[index] = other;
    }
  }

  FText GetDisplayText() { return FText::FromName(CategoryName); }
  FText GetTooltipText() { return FText::GetEmpty(); }
  void  GetChildren(TArray<UItem*>& OutChildren) { OutChildren.Append(Items); }

protected:
  UPROPERTY()
  FName CategoryName;

  UPROPERTY()
  TArray<UItem*> Items;
};

UCLASS(Blueprintable)
class UItemDatabase : public UObject {
  GENERATED_UCLASS_BODY()

public:
  const TArray<UItemCategory*>& GetCategoryList() { return ItemCategoryList; }

  UItemCategory* AddCategory();

  void DeleteCategory(UItemCategory* Category);
  void MoveUpCategory(UItemCategory* Category);

  UItem* AddItemToCategory(FName CategoryName);
  UItem* AddItemToCategory(UItemCategory* Category);

  void RemoveItemFromCategory(UItemCategory* Category, UItem* Item);
  void RemoveItem(UItem* Item);
  void MoveUpItem(UItem* Item);

private:
  UPROPERTY()
  TArray<UItemCategory*> ItemCategoryList;

  UItemCategory* FindCategory(FName CategoryName);

  void SaveToUndoHistory(FName name);
};
